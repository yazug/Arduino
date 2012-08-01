#include "Wire.h"
#include "PN532_I2C.h"
#include "ISO14443Card.h"

#include "Monitor.h"

#define IRQ   (2)
#define RESET (0xff)  // Not connected by default on the NFC Shield
PN532 nfc(PN532::I2C_ADDRESS, IRQ, RESET);

Monitor mon(Serial);

const byte IizukaKey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const byte factory_a[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

long prev;
ISO14443Card card;

byte pollingTypes[] = {
  2,
  nfc.Type_GenericPassiveTypeA,
  nfc.Type_GenericPassive212kbFeliCa
};


void setup() {
  mon.begin(57600);

  Wire.begin();
  nfc.begin();

  mon << "Hi.";
  PN532_init();
  delay(500);

}

void loop() {
  ISO14443Card tcard;
  byte c, tmp[64];

  if ( millis() > prev + 50 ) {
    prev = millis();
    if ( nfc.InAutoPoll(2,2,pollingTypes+1,pollingTypes[0]) != 0 && nfc.autoPoll_response(tmp) != 0 ) {
      mon << mon.endl << "Num of tags = " << (int)tmp[0] << mon.endl;
      PN532::printHexString(tmp, 16);
      if ( tmp[0] == 1 ) {
        tcard.set(tmp[1], tmp+3);
        mon << "type = " << tcard.type << mon.endl;
      }
      if ( tcard != card ) { 
        card = tcard;
        if ( card.type == 0x11 ) {
          mon << "FeliCa" << mon.endl 
            << " ID: " << mon.printHexString( card.IDm, 8) << mon.endl;
          mon << "Pad: " << mon.printHexString( card.PMm, 8) << mon.endl;
          //        mon << mon.printHexString( card.SystemCode(), 2) << mon.endl;
          int len;
          // Polling command, with system code request.
          word syscode = 0x00FE;
          len = nfc.felica_Polling(tmp, syscode);
          mon << "System code " << (syscode>>4 & 0x0f) <<  (syscode & 0x0f) 
              << (syscode>>12& 0x0f) << (syscode >> 8 & 0x0f) 
              <<" specific IDm " << mon.printHexString(tmp, 8) << mon.endl;
          // low-byte first service code.
          // Suica, Nimoca, etc. 0x090f system 0x0300
          // Edy service 0x170f (0x1317), system 0x00FE // 8280
          // FCF 1a8b
          mon << "Request Service code and read blocks: " << mon.endl;
          word scodes[] = { 
            0x1a8b, 0x170f, 0x090f, 0xffff
          };
          int snum = 1;
          for(int i = 0; i < snum; i++) {
            word scver = nfc.felica_RequestService(0x1a8b);
            mon << mon.printHexString(scodes[i]) << ": " 
              << mon.printHexString(scver) << mon.endl;
            if ( scodes[i] != 0xffff && scver != 0xffff ) {
              word blks[1];
              for (word blkno = 0; blkno < 4; blkno++) {
                blks[0] = blkno;
                c = nfc.felica_ReadWithoutEncryption(tmp, scodes[i], 1, blks);
                mon << mon.printHexString((byte*) &blks[0], 2) << ": ";
                if ( c != 0 ) {
                  mon << mon.printHexString(tmp, c*16) << "  ";
                  mon.print(tmp, c*16, 255);
                  mon << mon.endl;
                }
                //mon << mon.endl;
              }
            }
          }
        } 
        else if ( card.type == 0x10 ) {
          mon << "Mifare" << mon.endl << "  ID: ";
          mon.printHexString(card.UID, card.IDLength);
          mon << mon.endl;
          if ( nfc.mifare_AuthenticateBlock(card.UID, card.IDLength, 4,
          IizukaKey_b) ) {
            mon << "Auth Success." << mon.endl;
            nfc.mifare_ReadDataBlock(4, tmp);
            mon << mon.printHexString(tmp, 16) << "  ";
            mon << mon.print(tmp, 16, 255) << mon.endl;
            nfc.mifare_ReadDataBlock(5, tmp);
            mon << mon.printHexString(tmp, 16) << "  ";
            mon << mon.print(tmp, 16, 255) << mon.endl;
            nfc.mifare_ReadDataBlock(6, tmp);
            mon << mon.printHexString(tmp, 16) << "  ";
            mon << mon.print(tmp, 16, 255) << mon.endl;
          } 
          else {
            mon << "Failure." << mon.endl;
          }
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
    mon << "Couldn't find PN53x on Wire." << mon.endl;
    while (1); // halt
  } 
  else {
    mon << mon.setbase(HEX) << mon.printHexString((byte*)&r, 4) << mon.endl;
  }

  // Got ok data, print it out!
  Serial.print("Found chip PN5"); 
  Serial.println(r & 0xff, HEX);
  Serial.print("Firmware ver. "); 
  Serial.print(r>>8 & 0xFF, DEC); 
  Serial.print('.'); 
  Serial.println(r>>16 & 0xFF, DEC);

  Serial.println("SAMConfiguration");
  nfc.SAMConfiguration();

}





























