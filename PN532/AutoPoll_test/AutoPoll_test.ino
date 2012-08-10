#include "Wire.h"
#include "PN532_I2C.h"
#include "ISO14443.h"

#include "Monitor.h"
char tmp[80];
Monitor mon(Serial);

#define IRQ   (2)
#define RESET (0xff)  // Not connected by default on the NFC Shield
PN532 nfc(PN532::I2C_ADDRESS, IRQ, RESET);

const byte IizukaKey[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const byte FactoryKey[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

long prev;
ISO14443 card;

byte polltypes[] = {
  2,
  TypeA,
  TypeF
};

void setup() {
  Serial.begin(57600);
  Wire.begin();
  nfc.begin();

  mon << "Hi." << endl;
  reader_init();

  delay(500);
}

void loop() {
  ISO14443 tmpcard;
  byte c, buf[64];

  if ( millis() > prev + 50 ) {
    prev = millis();
    if ( nfc.autoPoll(polltypes, buf) != 0 ) {
      c = buf[0];
      Serial << endl << c << " Tag, ";
      if ( c ) {
        card.init();
        card.set(buf[1], buf+3);
        Serial << "type = " << card.typeString((char*)buf, card.type) 
          << endl;
        mon.printHex((byte*)card.id, card.IDLength); mon << endl;
      }
      if ( card.type == 0x11 ) {
        Serial << "FeliCa" << endl 
          << " ID: "; mon.printHex( card.id, card.IDLength); mon << endl;
        Serial << "Pad: "; mon.printHex( card.PMm, card.IDLength); mon << endl;
        //        mon << mon.printHexString( card.SystemCode(), 2) << mon.endl;

        int len;
        // Polling command, with system code request.
        len = nfc.felica_Polling(buf, 0x00fe);
        mon << "Polling "; mon.print((word)0x00fe, HEX); mon << " ..." ;
//        Serial.flush();
//        Serial << mon.printHex(buf, len) << endl;
        Serial << "Request Service code: ";
        word scode = 0x1a8b; //, 0x170f, 0x1317, 0x1713, 0x090f, 0xffff 
        int snum = 0;
        word blklist[] = { 0,1,2,3 };
        word codever = nfc.felica_RequestService(scode);
        mon.print(scode, HEX); 
        mon << ": ";
        mon.print(codever, HEX); mon << endl;
        if ( codever != 0xffff && scode != 0xffff) {
          c = nfc.felica_ReadBlocksWithoutEncryption(buf, scode, (byte) 4, blklist);
          if ( c != 0 ) {
            mon.printHex(buf, 16); mon << endl;
            mon.printHex(buf+16, 16); mon << endl;
            mon.printHex(buf+32, 16); mon << endl;
            mon.printHex(buf+48, 16); mon << endl;
            //mon.printHex(tmp, 16, 255);
            Serial << endl;
          }
        }
        /*
        for(int i = 0; i < snum; i++) {
         word scver = nfc.felica_RequestService(scodes[i]);
         mon << mon.printHexString(scodes[i]) << ": " 
         << mon.printHexString(scver) << mon.endl;
         if ( scodes[i] != 0xffff && scver != 0xffff ) {
         byte blks[] = { 
         0x80, 0x00, 0x80, 0x01, 0x80, 0x02, 0x80, 0x03                                                };
         for (int blkno = 0; blkno < 4; blkno++) {
         c = nfc.felica_ReadWithoutEncryption(tmp, scodes[i], 1, blks+(blkno*2));
         mon << mon.printHexString(blks+(blkno*2), 2) << ": ";
         if ( c != 0 ) {
         mon << mon.printHexString(tmp, c*16) << mon.endl;
         mon.print(tmp, c*16, 255);
         mon << mon.endl;
         }
         mon << mon.endl;
         }
         }
         }
         */
      } 
      else if ( card.type == 0x10 ) {
        Serial << "Mifare" << endl << "  ID: ";
        mon.printHex(card.UID, card.IDLength);
        mon << endl;
        /*
       if ( nfc.mifare_AuthenticateBlock(card.UID(), card.IDLength(), 4,
         factory_a) ) {
         mon << "Success." << mon.endl;
         nfc.mifare_ReadDataBlock(4, tmp) << mon.endl;
         } 
         else {
         mon << "Failure." << mon.endl;
         }
         */
      }
    } 
  }
  delay(1000);
}


void reader_init() {
  unsigned long r;
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
    // Got ok data, print it out!
    mon << "Found chip PN5";
    mon.print((byte) (r & 0xff));
    mon  << ", Firmware ver. " << (r>>8 & 0xFF) << '.' << (r>>16 & 0xFF) << endl;
  }

  Serial << "SAMConfiguration...";
  nfc.SAMConfiguration();
  Serial << " finished." << endl;
}






