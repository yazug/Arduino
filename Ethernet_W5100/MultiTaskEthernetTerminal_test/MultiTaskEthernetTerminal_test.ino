#include <SPI.h>
#include <EEPROM.h>
#include "Ethernet_SPI.h"
#include "DS3234.h"
#include "Wire.h"
#include "PN532_I2C.h"
#include "ISO14443.h"
#include "SD_SPI.h"
#include "SPISRAM.h"

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
IPAddress ip(192,168,1, 177);
IPAddress gateway(192,168,1, 1);
IPAddress subnet(255, 255, 255, 0);
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

const int SD_CS = 4;    
SDClass sd(SD_CS);
SdVolume volume;
SdFile root;

const int SRAM_CS = 8;
SPISRAM spisram(SRAM_CS);

byte buf[128];

long stwatch;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(19200);

  mon << "Ethernet terminal test @ port 1234. " << endl;
  //   while (!Serial) {
  //    ; // wait for serial port to connect. Needed for Leonardo only
  //  }

  SPI.begin();

  if (!sd.begin()) {
    mon << "SD Card initialization failed." << endl;
  }
  spisram.begin();

  rtc_spi.begin();
  // start the Ethernet connection and the server:
  rom_macaddr();
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  mon << "server is at " << Ethernet.localIP() << endl;

  Wire.begin();
  nfc.begin();
  PN532_init();

  card.clear();
  cardlog.init();

  mon << "initialized." << endl;

  buf[0] = 0;
}


void loop() {
  // listen for incoming clients
  //  char c;
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
      mon.print(rtc_spi.time, HEX);
      mon << " ";
      mon.print(rtc_spi.cal, HEX);
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
      if ( client.connected() && client.available() ) {
        Monitor cmon(client);
        if ( cmon.readLine((char*)buf, 127) > 0 ) {
          for(int i = 0; buf[i] ; i++) 
            buf[i] = toupper(buf[i]);
          len = cmon.ithToken((const char*)buf, 0, pos);
          len = min(len, 8);
          if ( len > 0 && strncmp((char*)buf+pos,"INFO", len) == 0 ) {
            cmon << "INFO >> ";
            cmon.printHex(mac, 6);
            cmon << ", ";
            long ipval = (unsigned long)ip;
            cmon.print(ipval & 0xff);
            cmon.print('.');
            cmon.print(ipval>>8 & 0xff);
            cmon.print('.');
            cmon.print(ipval>>16 & 0xff);
            cmon.print('.');
            cmon.print(ipval>>24 & 0xff);
            cmon << endl;
            //
          } 
          else if ( len > 0 && strncmp((char*)buf+pos, "TIME", len) == 0 ) {
            cmon << ">> " << "TIME ";
            cmon.print(rtc_spi.cal, HEX);
            cmon << " - ";
            cmon.print(rtc_spi.time>>16&0xff, HEX);
            cmon << ":";
            cmon.print(rtc_spi.time>>8&0xff, HEX);
            cmon << ":";
            cmon.print(rtc_spi.time&0xff, HEX);
            cmon << endl;
            mon << "TIME command. " << endl;
          } 
          else if ( len > 0 && strncmp((char*)buf+pos, "TSET", len) == 0 ) {
            len = cmon.ithToken((const char*)buf, 1, pos);
            if ( len ) {
              buf[pos+len] = 0;
              cmon << ">> " << "SET TIME TO " << ((char*)buf+pos) << endl;
              current = strtol((const char*) buf+pos, NULL, HEX);
              rtc_spi.setTime(current);
            }
          } 
          else if ( len > 0 && strncmp((char*)buf+pos, "CSET", len) == 0 ) {
            len = cmon.ithToken((const char*)buf, 1, pos);
            if ( len ) {
              buf[pos+len] = 0;
              cmon << ">> " << "SET CALENDAR TO " << ((char*)buf+pos) << endl;
              current = strtol((const char*) buf+pos, NULL, HEX);
              rtc_spi.setCalendar(current);
            }
          }
          else if ( len > 0 && strncmp((char*)buf+pos, "LIST", len) == 0 ) {
            server_status = LISTING;
            listix = 0;
          } 
          else if ( len > 0 && strncmp((char*)buf+pos, "DELETE", len) == 0 ) {
            len = cmon.ithToken((const char*)buf, 1, pos);
            if ( len ) {
              buf[pos+len] = 0;
              cmon << ">> " << "DELETE ENTRIES";
              current = strtol((const char*) buf+pos, NULL, HEX);
              current = cardlog.remove(current);
              cmon << " " << current << ". " << endl;
            }
          } 
          else if ( len > 0 && strncmp((char*)buf+pos, "QUIT", len) == 0 ) {
            cmon << ">> " << "bye bye." << endl;
            delay(5);
            client.stop();
            server_status = DISCONNECTED;
            mon << "disonnected client." << endl;
          }
          buf[0] = 0;
          client_idle_since = millis();
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
      cmon.printHex(cardlog[listix].NFCID, 8, '-');
      cmon << " ";
      cmon.printHex(cardlog[listix].PIN, 8);
      cmon << endl;
    }
    if ( !(listix < cardlog.count()) )
      // if finished.
      server_status = CONNECTED;
  }
}

void rom_macaddr() {
  if ( EEPROM.read(0) == 0x9e && EEPROM.read(1) ) {
    for(int i = 0; i < 6; i++) {
      mac[i] = EEPROM.read(2+i);
    }
  }
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




