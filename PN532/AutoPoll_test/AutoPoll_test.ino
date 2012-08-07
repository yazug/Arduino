#include "Wire.h"
#include "PN532_I2C.h"
#include "ISO14443.h"

//#include "Monitor.h"
#include <string.h>

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

template<class T>
inline Stream &operator <<(Stream &stream, T arg) {
  stream.print(arg);
  return stream;
}
const char endl = '\n';

char printbuf[32];
char * printHex(byte * a, int len, char * buf = printbuf) {
  for(int i = 0; i < len; i++) {
    sprintf(buf, "%02X ", a[i]);
  }
  return buf;
}
char * printHex(unsigned long v, char * buf = printbuf) {
  sprintf(buf, "%08X ", v);
  return buf;
}
char * printHex(byte v, char * buf = printbuf) {
  sprintf(buf, "%02X ", v);
  return buf;
}


char buf[40];

void setup() {
  unsigned long r = 0;

  Serial.begin(57600);
  Wire.begin();
  nfc.begin();

  Serial << "Hi." << endl;
  for (int i = 0; i < 10 ; i++) {
    if ( (r = nfc.getFirmwareVersion()) )
      break;
    delay(250);
  }
  if (! r ) {
    Serial << "Couldn't find PN53x on Wire." << endl;
    while (1); // halt
  } 
  else {
    Serial << "Firmware version: ";
    sprintf(buf, "%04Xh", r);
    Serial << buf << endl;
    // Got ok data, print it out!
    Serial << "Found chip PN5" << printHex(static_cast<byte>(r & 0xff)) << ", Firmware ver. " 
     << (r>>8 & 0xFF) << '.' << (r>>16 & 0xFF) << endl;
  }

  Serial << "SAMConfiguration";
  nfc.SAMConfiguration();
  Serial << " finished." << endl;

  delay(500);

}

void loop() {
  ISO14443 tmpcard;
  byte c, tmp[64];

  if ( millis() > prev + 50 ) {
    prev = millis();
    tmp[0] = nfc.Type_GenericPassiveTypeA;
    tmp[1] = nfc.Type_GenericPassive212kbFeliCa;
    tmp[2] = nfc.Type_GenericPassive424kbFeliCa;
    tmp[3] = nfc.Type_PassiveTypeB;
    if ( nfc.autoPoll(polltypes, tmp) != 0 ) {
      Serial << endl << (int)tmp[0] << " Tag, ";
      card.init();
      if ( tmp[0] ) {
        Serial << printHex((byte*)tmp, 16) << endl;
        card.set(tmp[1], tmp+3);
        Serial << "type = " << card.typeString(buf, card.type) << endl;
        Serial << printHex((byte*)card.IDm, 8) << endl;
      }
      /*
      if ( card.type == 0x11 ) {
       Serial << "FeliCa" << endl 
       << " ID: " << mon.printHexString( card.IDm(), 8) << mon.endl;
       mon << "Pad: " << mon.printHexString( card.PMm(), 8) << mon.endl;
       //        mon << mon.printHexString( card.SystemCode(), 2) << mon.endl;
       int len;
       // Polling command, with system code request.
       len = nfc.felica_Polling(tmp, 0x00fe);
       mon << mon.printHexString(tmp, len) << mon.endl;
       //
       mon << "Request System Code: " << mon.endl;
       len = nfc.felica_RequestSystemCode(tmp);
       mon << mon.printHexString((word *)tmp, len) <<mon.endl;
       // low-byte first service code.
       // Suica, Nimoca, etc. 0x090f system 0x0300
       // Edy service 0x170f (0x1317), system 0x00FE // 8280
       // FCF 1a8b
       mon << "Request Service code and read blocks: " << mon.endl;
       word scodes[] = { 
       0x1a8b, 0x170f, 0x1317, 0x1713, 0x090f, 0xffff                                         };
       int snum = 5;
       for(int i = 0; i < snum; i++) {
       word scver = nfc.felica_RequestService(scodes[i]);
       mon << mon.printHexString(scodes[i]) << ": " 
       << mon.printHexString(scver) << mon.endl;
       if ( scodes[i] != 0xffff && scver != 0xffff ) {
       byte blks[] = { 
       0x80, 0x00, 0x80, 0x01, 0x80, 0x02, 0x80, 0x03                                    };
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
       } 
       else if ( card.type == 0x10 ) {
       mon << "Mifare" << mon.endl << "  ID: ";
       mon.printHexString(card.UID(), card.IDLength());
       mon << mon.endl;
       if ( nfc.mifare_AuthenticateBlock(card.UID(), card.IDLength(), 4,
       factory_a) ) {
       mon << "Success." << mon.endl;
       nfc.mifare_ReadDataBlock(4, tmp) << mon.endl;
       } 
       else {
       mon << "Failure." << mon.endl;
       }
       }
       */
    } 
  }
  delay(1000);
}






























