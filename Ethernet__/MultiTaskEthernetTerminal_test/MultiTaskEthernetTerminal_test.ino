#include <SPI.h>
#include <EEPROM.h>
#include "Ethernet.h"
#include "DS3234.h"
#include "Wire.h"
#include "PN532_I2C.h"
#include "ISO14443.h"
#include <sd_raw.h>

#include "Monitor.h"

#include "IDCard.h"
#include "queue.h"

PN532 nfc(PN532::I2C_ADDRESS, 2, 7);
byte polling[] = { 
  2, TypeF, TypeA };
long lastpoll;
long lastread;
enum {
  IDLE,
  POLLING,
  RECEIVED_NFCID,
  AUTHENTICATED,
  ERROR
} 
reader_status = IDLE;

ISO14443 card;
Queue cardlog;

byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetServer server(1234);
Monitor mon(Serial);
EthernetClient client;
long client_idle_since = 0;
enum {
  DISCONNECTED,
  CONNECTED,
  LISTING
} 
server_status = DISCONNECTED;
int listix;


DS3234 rtc_spi(9);
long lastrtcupdate;

MemCard sd(4);

byte buf[128];
byte * bufp;

long stwatch;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(19200);
  mon << "Ethernet terminal test @ port 1234. " << endl;
  //   while (!Serial) {
  //    ; // wait for serial port to connect. Needed for Leonardo only
  //  }

  SPI.begin();
  rtc_spi.begin();
  // start the Ethernet connection and the server:
  init_Ethernet();
  server.begin();
  mon << "server is at " << Ethernet.localIP() << endl;

  sd.begin();
  if ( !print_disk_info() ) mon << "error!" << endl;  

  Wire.begin();
  nfc.begin();
  PN532_init();

  card.clear();
  cardlog.init();

  mon << "initialize finished." << endl;
  
  bufp = buf;
  *bufp = 0;
}


void loop() {
  // listen for incoming clients
  char c;
  byte cnt;
  long current;
  int pos, len;
  word blklist[] = { 
    0, 
    1, 
    2, 
    3
  };

  if ( millis() > lastrtcupdate + 133 ) {
    current = rtc_spi.time;
    rtc_spi.update();
    if ( current != rtc_spi.time ) {
      rtc_spi.printTimeOn(mon);
      mon << " ";
      rtc_spi.printCalendarOn(mon);
      mon << endl;
    }
  }

  stwatch = millis();
  switch(reader_status) {
  case IDLE:
    if (millis() < lastpoll + 101 ) 
      break;
    if ( card.type != Type_Empty && millis() > lastread + 5000 )
      card.clear();
    if ( nfc.InAutoPoll(1, 1, polling+1, polling[0]) ) {
      lastpoll = millis();
      reader_status = POLLING;
    }
    else {
      reader_status = ERROR;
    }
    break;

  case POLLING:
    if ( !nfc.IRQ_ready() ) 
      break;
    cnt = nfc.getAutoPollResponse((byte*) buf);
    if ( nfc.status() != nfc.RESP_RECEIVED ) {
      reader_status = ERROR;
      break;
    }
    if ( cnt == 0 ) {
      reader_status = IDLE;
      break;
    }
    if (card.type == nfc.target.NFCType 
      && memcmp(card.ID, nfc.target.IDm, nfc.target.IDLength) == 0 ) {
      //mon << "hey" << endl;
      reader_status = IDLE;
      lastpoll = millis() + 1000;
      break;
    }
    reader_status = RECEIVED_NFCID;
    card.set(nfc.target.NFCType, nfc.target.IDm, nfc.target.IDLength);
    card.printOn(mon);
    mon << endl;
    break;

  case RECEIVED_NFCID:
    switch(nfc.target.NFCType) {
    case FeliCa212kb:
      if ( nfc.felica_Polling(buf, 0x00fe) && (cnt = nfc.felica_RequestService(0x1a8b) != 0xffff) ) {
        mon << "authenticaterd." << endl;
        reader_status = AUTHENTICATED;
      }
      break;
    case Mifare:
      if ( nfc.mifare_AuthenticateBlock(4, KawazuKey_b) 
        && nfc.getCommandResponse(buf) && buf[0] == 0 ) {
        mon << "authenticaterd." << endl;
        reader_status = AUTHENTICATED;
      }    
      break;
    default:
      mon << "Card is unknown type" << endl;
      break;
    }
    if ( reader_status != AUTHENTICATED ) {
      mon << "Failed authentication " << endl;
      reader_status = IDLE;
    }
    break;

  case AUTHENTICATED:
    switch (card.type) {
    case FeliCa212kb:
      if ( nfc.felica_ReadBlocksWithoutEncryption(buf, 0x1a8b, (byte) 4, blklist) ) {
        IDCardData & idcard((IDCardData &) buf);
        mon << "PIN ";
        mon.printHex(idcard.felica.pin, 8);
        mon << " ISSUE " << idcard.felica.issue << endl;
        cardlog.add(rtc_spi.cal, rtc_spi.time, card.type, card.IDm, idcard.felica.pin);
        //mon << cardlog.count() << endl;
      }
      break;
    case Mifare:
      if ( nfc.mifare_ReadDataBlock(4, buf) ) {
        IDCardData & idcard((IDCardData &) buf);
        mon << "PIN ";
        mon.printHex(idcard.mifare.pin, 8);
        mon << " ISSUE " << idcard.mifare.issue << endl;
        cardlog.add(rtc_spi.cal, rtc_spi.time, card.type, card.UID, idcard.felica.pin);
        //mon << cardlog.count() << endl;
      }
      break;
    }
    lastread = millis();
    reader_status = IDLE;
    break;
  default:
    // mon << "reader_status = " << (byte) reader_status << endl;
    reader_status = IDLE;
    break;
  }
  stwatch = millis() - stwatch;
  if ( stwatch > 250 )
    mon << "Task > 250 msec " << stwatch << " millis to " << (byte) reader_status << endl;


  if ( !client ) {
    if ( client = server.available() ) {
      client_idle_since = millis();
      server_status = CONNECTED;
      mon << "A New client started." << endl;
    }
  } 
  else if ( server_status == CONNECTED ) {
    if (millis() < client_idle_since + 30000L  ) {
      // an http request ends with a blank line
      if (client.connected()) {
        if (client.available()) {
          Monitor cmon(client);
          if ( cmon.readLine((char*)bufp, 127) ) {
            for(int i = 0; buf[i] ; i++) 
              buf[i] = toupper(buf[i]);
            switch( *((long*)buf) ) {
            case 0x52444441: //ADDR
              cmon << ">> " << "ADDR ";
              cmon.printHex(mac, 6);
              cmon << endl;
              mon << "MAC command. " << endl;
              //
              break;
            case 0x454D4954: // TIME
              cmon << ">> " << "TIME ";
              rtc_spi.printCalendarOn(cmon);
              cmon << " - ";
              rtc_spi.printTimeOn(cmon);
              cmon << endl;
              mon << "TIME command. " << endl;
              //
              break;

            case 0x54455354: // tset
            case 0x54455343: // cset
              len = cmon.ithToken((const char*)buf, 1, pos);
              if ( !len ) break;
              buf[pos+len] = 0;
              if ( buf[0] == 'T' ) {
                cmon << ">> " << "SET TIME TO " << ((char*)buf+pos) << endl;
                current = strtol((const char*) buf+pos, NULL, HEX);
                rtc_spi.setTime(current);
              } 
              else {
                cmon << ">> " << "SET CALENDAR TO " << ((char*)buf+pos) << endl;
                current = strtol((const char*) buf+pos, NULL, HEX);
                rtc_spi.setCalendar(current);
              }
              break;

            case 0x5453494C: // LIST
              server_status = LISTING;
              listix = 0;
              break;
              
              case 0x544C4544: // DELT
                len = cmon.ithToken((const char*)buf, 1, pos);
                if ( !len ) break;
                buf[pos+len] = 0;
                cmon << ">> " << "DELETE ENTRIES";
                current = strtol((const char*) buf+pos, NULL, HEX);
                current = cardlog.remove(current);
               cmon << " " << current << ". " << endl;
                break;

            case 0x54495551: //QUIT
              cmon << ">> " << "bye bye." << endl;
              delay(5);
              client.stop();
              server_status = DISCONNECTED;
              mon << "disonnected client." << endl;
              //
              break;

            }
            bufp = buf;
            *bufp = 0;
            client_idle_since = millis();
          }
        }
      }
      // close the connection:
    } 
    else {
      client.stop();
      mon << "disonnected client by timeout." << endl;
    }
  } 
  else if ( server_status == LISTING ) {
    current = millis();
    Monitor cmon(client);
    for( /* value is set already */; 
    listix < cardlog.count() && millis() < current + 200 ; listix++) {
      if ( listix == 0 ) {
        cmon << "No  Date   TIME TYPE     Card ID           PIN";
        cmon << endl;
      }
      cmon << listix+1 << "  ";
      cmon.print(cardlog[listix].date, HEX);
      cmon << " ";
      cmon.print(cardlog[listix].time, HEX);
      cmon << " ";
      cmon.print(cardlog[listix].type, HEX);
      cmon << " ";
      cmon.printHex(cardlog[listix].CID, 8, '-');
      cmon << " ";
      cmon.printHex(cardlog[listix].PIN, 8);
      cmon << endl;
    }
    if ( !(listix < cardlog.count()) )
      // if finished.
      server_status = CONNECTED;
  }
}

void init_Ethernet() {
  if ( EEPROM.read(0) == 0x9e && EEPROM.read(1) ) {
    for(int i = 0; i < 6; i++) {
      mac[i] = EEPROM.read(2+i);
    }
  }
  IPAddress ip(192,168,1, 177);
  IPAddress gateway(192,168,1, 1);
  IPAddress subnet(255, 255, 0, 0);
  Ethernet.begin(mac, ip, gateway, subnet);
}

boolean PN532_init() {
  byte cnt = 0;
  for (int i = 0; i < 3  && !( nfc.GetFirmwareVersion() && (cnt = nfc.getCommandResponse((byte*)buf)) ); i++) 
    delay(500);
  if (! cnt ) {
    Serial << "Couldn't find PN53x on Wire." << endl;
    return false;
  } 
  Serial << "PN53x IC ver. " << (char)buf[0] << ", Firmware ver. " 
    << (byte) buf[1] << '.' << (byte) buf[2] << endl;

  if ( nfc.SAMConfiguration() && nfc.getCommandResponse((byte*)buf) 
    && nfc.status() == PN532::RESP_RECEIVED) {
    Serial << "SAMConfiguration," << endl;
  }
  nfc.CPU_PowerMode(2);
  nfc.getCommandResponse((byte*)buf);
  reader_status = IDLE;
  return true;
}







int print_disk_info()
{

  MemCard::sd_raw_info disk_info;
  if(!sd.get_info(&disk_info))
  {
    return 0;
  }

  Serial.println();
  Serial.print("rev:    ");
  Serial.print(disk_info.revision,HEX);
  Serial.println();
  Serial.print("serial: 0x");
  Serial.print(disk_info.serial,HEX);
  Serial.println();
  Serial.print("date:   ");
  Serial.print(disk_info.manufacturing_month,DEC);
  Serial.println();
  Serial.print(disk_info.manufacturing_year,DEC);
  Serial.println();
  Serial.print("size:   ");
  Serial.print(disk_info.capacity,DEC);
  Serial.println();
  Serial.print("copy:   ");
  Serial.print(disk_info.flag_copy,DEC);
  Serial.println();
  Serial.print("wr.pr.: ");
  Serial.print(disk_info.flag_write_protect_temp,DEC);
  Serial.print('/');
  Serial.print(disk_info.flag_write_protect,DEC);
  Serial.println();
  Serial.print("format: ");
  Serial.print(disk_info.format,DEC);
  Serial.println();
  Serial.print("free:   ");

  return 1;
}









