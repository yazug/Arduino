#include "Wire.h"
#include "PN532_I2C.h"
#include "ISO14443.h"

#include <Monitor.h>
Monitor mon(Serial);

const char endl = '\n';
#define IRQ   (2)
#define RESET (0xff)  // Not connected by default on the NFC Shield
PN532 nfc(PN532::I2C_ADDRESS, IRQ, RESET);

const byte IizukaKey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const byte factory_a[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

long prev;
ISO14443 card;

byte pollingTypes[] = {
  2,
  nfc.Type_GenericPassiveTypeA,
  nfc.Type_GenericPassive212kbFeliCa
};


void setup() {
  Serial.begin(57600);

  Wire.begin();
  nfc.begin();

  Serial << "Hi." << endl;
  PN532_init();
  delay(500);

}

void loop() {
  ISO14443 tcard;
  byte c, tmp[64];

  if ( millis() > prev + 50 ) {
    prev = millis();
    if ( nfc.InAutoPoll(2,2,pollingTypes+1,pollingTypes[0]) != 0 
        && nfc.getAutoPollResponse(tmp) != 0 ) {
      Serial << endl << "Num of tags = " << (int)tmp[0] << endl;
      mon.printArray(tmp, 16);
      if ( tmp[0] == 1 ) {
        tcard.set(tmp[1], tmp+3);
        Serial << "type = " << tcard.type << endl;
      }
      if ( tcard != card ) { 
        card = tcard;
        if ( card.type == 0x11 ) {
          Serial << "FeliCa" << endl 
            << " ID: ";
          mon.printArray( card.IDm, 8);
          Serial << endl;
          Serial << "Pad: ";
          mon.printArray( card.PMm, 8);
          Serial << endl;
          //        mon << mon.printHexString( card.SystemCode(), 2) << mon.endl;
          int len;
          // Polling command, with system code request.
          word syscode = 0x00FE;

          len = nfc.felica_Polling(tmp, syscode);
          Serial << "System code ";
          Serial.print(syscode, HEX); 
          Serial    <<" specific IDm ";
          mon.printArray(tmp, 8);
          Serial << endl;

          // low-byte first service code.
          // Suica, Nimoca, etc. 0x090f system 0x0300
          // Edy service 0x170f (0x1317), system 0x00FE // 8280
          // FCF 1a8b
          Serial << "Request Service code and read blocks: " << endl;
          word servcode = 0x1a8b;
          word scver = nfc.felica_RequestService(servcode);
          Serial.print(servcode, HEX);
          Serial << ": "; 
          Serial.println(scver, HEX);
          if ( scver != 0xffff ) {
            word blks[1];
            for (word blkno = 0; blkno < 4; blkno++) {
              blks[0] = blkno;
              c = nfc.felica_ReadWithoutEncryption(tmp, servcode, 1, blks);
              mon.printArray((byte*) &blks[0], 2);
              Serial << ": ";
              if ( c != 0 ) {
                mon.printArray(tmp, c*16);
                Serial << "  ";
                mon.printArray((char*) tmp, c*16);
                Serial << endl;
              }
              //mon << mon.endl;
            }
          }
        }
      } 
      else if ( card.type == 0x10 ) {
        Serial << "Mifare" << endl << "  ID: ";
        mon.printArray(card.UID, card.IDLength);
        Serial << endl;
        if ( nfc.mifare_AuthenticateBlock(card.UID, card.IDLength, 4,
        IizukaKey_b) ) {
          Serial << "Auth Success." << endl;
          nfc.mifare_ReadDataBlock(4, tmp);
          mon.printArray(tmp, 16); 
          Serial << endl;
          nfc.mifare_ReadDataBlock(5, tmp);
          mon.printArray(tmp, 16); 
          Serial << endl;
          nfc.mifare_ReadDataBlock(6, tmp);
          mon.printArray(tmp, 16); 
          Serial << endl;
        } 
        else {
          Serial << "Failure." << endl;
        }
      }
    } 
  }
  delay(250);
}

void PN532_init() {

  mon << "Firmware version: ";
  unsigned long r = 0;
  for (int i = 0; i < 10 ; i++) {
    if ( (r = nfc.getFirmwareVersion()) )
      break;
    delay(250);
  }
  if (! r ) {
    mon << "Couldn't find PN53x on Wire." << endl;
    while (1); // halt
  } 
  else {
    mon.printArray((byte*)&r, 4); 
   mon << endl;
  }

  // Got ok data, print it out!
  Serial << "Found chip PN5";
  Serial.println(r & 0xff, HEX);
  Serial << "Firmware ver. " << (r>>8 & 0xFF) << '.' 
  << (r>>16 & 0xFF) << endl;

  Serial << "SAMConfiguration" << endl;
  
  nfc.SAMConfiguration();

}




