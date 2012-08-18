#include <SPI.h>
#include <EEPROM.h>
#include "Ethernet.h"
#include "DS3234.h"
#include "Wire.h"
#include "PN532_I2C.h"
#include "ISO14443.h"

#include "Monitor.h"

#include "IDCard.h"

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

byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetServer server(1234);
Monitor mon(Serial);
EthernetClient client;

DS3234 rtc_spi(9);
long lastrtcupdate;

byte buf[128];
byte * bufp;

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

  Wire.begin();
  nfc.begin();
  PN532_init();

  card.clear();

  bufp = buf;
  *bufp = 0;
}


void loop() {
  // listen for incoming clients
  char c;
  byte cnt;
  long current;
  ISO14443 tmpcard;

  if ( millis() > lastrtcupdate + 133 ) {
    current = rtc_spi.time;
    rtc_spi.update();
    if ( current != rtc_spi.time ) {
      rtc_spi.printTimeOn(mon);
      mon << endl;
    }
  }

  if ( reader_status == IDLE ) {
    if ( card.type != Type_Empty && millis() > lastread + 6000 ) 
      card.clear();
    if ( millis() > lastpoll + 101 ) {
      if ( nfc.InAutoPoll(1, 1, polling+1, polling[0]) ) {
        lastpoll = millis();
        // status goes to ACK_FRAME_RECEIVED
        reader_status = POLLING;
      }
      else {
        mon << "AutoPoll Request Error. " << endl;
        reader_status = ERROR;
      }
    }
  } 
  else if ( reader_status == POLLING && nfc.IRQ_ready() ) {
    if ( nfc.getCommandResponse((byte*) buf) ) {
      if ( buf[0] == 1 ) {
        tmpcard.set(buf[1], buf+3);
        if ( card != tmpcard ) {
          card = tmpcard;
          reader_status = RECEIVED_NFCID;
          mon << "Card type " << card.type << ", ID "; 
          mon.printHex(card.id, card.IDLength); 
          mon << endl;
        } 
        else {
          reader_status = IDLE;
        }
      } 
      else {
        // No cards.
        reader_status = IDLE;
      }
    } 
  }
  else if ( reader_status == RECEIVED_NFCID ) {
    switch(card.type) {
    case FeliCa212kb:
      if ( nfc.felica_Polling(buf, 0x00fe) && (cnt = nfc.felica_RequestService(0x1a8b) != 0xffff) ) {
        mon << "Version of Service Code 0x1a8b " << cnt << endl;
        reader_status = AUTHENTICATED;
      }
      break;
    case Mifare:
      nfc.setUID(card.UID, card.IDLength, card.type);
      if ( nfc.mifare_AuthenticateBlock(4, KawazuKey_b) 
        && nfc.getCommandResponse(buf) && buf[0] == 0 ) {
        mon << "Mifare IizukaMagTape compat " << endl;
        reader_status = AUTHENTICATED;
      }    
      break;
    default:
      mon << "Card is unknown type" << endl;
      reader_status = IDLE;
      break;
    }
    lastread = millis();
    if ( reader_status != AUTHENTICATED ) {
      mon << "Failed authentication " << endl;
      reader_status = IDLE;
    }
  } 
  else if ( reader_status == AUTHENTICATED ) {
    word blklist[] = { 
      0, 1, 2, 3                 };
    switch (card.type) {
    case FeliCa212kb:
      if ( nfc.felica_ReadBlocksWithoutEncryption(buf, 0x1a8b, (byte) 4, blklist) ) {
        IDCardData & idcard((IDCardData &) buf);
        mon << "PIN ";
        mon.printHex(idcard.felica.pin, 8);
        mon << " ISSUE " << idcard.felica.issue << endl;
      }
      break;
    case Mifare:
      if ( nfc.mifare_ReadDataBlock(4, buf) ) {
        IDCardData & idcard((IDCardData &) buf);
        mon << "PIN ";
        mon.printHex(idcard.mifare.pin, 8);
        mon << " ISSUE " << idcard.mifare.issue << endl;
      }
      break;
    }
    reader_status = IDLE;
  }
  else {
    // mon << "reader_status = " << (byte) reader_status << endl;
    reader_status = IDLE;
  }


  if ( !client ) {
    if ( client = server.available() ) {
      mon << "new client opened." << endl;
    }
  } 
  else if ( client ) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    if (client.connected()) {
      if (client.available()) {
        Monitor cmon(client);
        if ( cmon.readLine((char*)bufp) ) {
          if ( strcmp("MAC", (char*)buf) == 0 ) {
            cmon << ">> " << "MAC ";
            cmon.printHex(mac, 6);
            cmon << endl;
            mon << "MAC command. " << endl;
            //
            bufp = buf;
            *bufp = 0;
          } 
          else 
            if ( strcmp("TIME", (char*)buf) == 0 ) {
            cmon << ">> " << "TIME ";
            rtc_spi.printCalendarOn(cmon);
            cmon << " - ";
            rtc_spi.printTimeOn(cmon);
            cmon << endl;
            mon << "TIME command. " << endl;
            //
            bufp = buf;
            *bufp = 0;
          }
          else 
            if ( strcmp("QUIT", (char*)buf) == 0 ) {
            cmon << ">> " << "bye bye." << endl;
            delay(5);
            client.stop();
            mon << "disonnected client." << endl;
            //
            bufp = buf;
            *bufp = 0;
          } 
          else {
            cmon << ">>" << (char*)buf << endl;
            //
            bufp = buf;
            *bufp = 0;
          }
        }
      }
    }
    // close the connection:
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
















