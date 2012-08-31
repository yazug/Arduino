#include "Wire.h"
#include "PN532_I2C.h"
#include "ISO14443.h"

#include "TextStream.h"

#define IRQ   (2)
#define RESET (0xff)  // Not connected by default on the NFC Shield
PN532 nfc(PN532::I2C_ADDRESS, IRQ, RESET);

TextStream mon(Serial);

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

  mon << "Hi.";
  PN532_init();
  delay(500);

}

void loop() {
  ISO14443 tcard;
  byte c, tmp[64];

  if ( millis() <= prev + 1000 ) 
    return;
  prev = millis();
  if ( nfc.InAutoPoll(2,2,pollingTypes+1,pollingTypes[0]) != 0 && nfc.getAutoPollResponse(tmp) != 0 ) {
    mon << endl << "Num of tags = " << (int)tmp[0] << endl;
    PN532::printHexString(tmp, 16);
    if ( tmp[0] == 1 ) {
      tcard.set(tmp[1], tmp+3);
      mon << "type = " << tcard.type << endl;
    }
    if ( tcard != card ) { 
      card = tcard;
      if ( card.type == 0x11 ) {
        mon << "FeliCa" << endl 
          << " ID: ";
        mon.printBytes( card.IDm, 8); mon << endl;
//        mon << "Pad: ";
//        mon.printBytes( card.PMm, 8); mon << endl;
        //        mon << mon.printHexString( card.SystemCode(), 2) << mon.endl;
        int len;
        word syscode = 0x00FE;
        nfc.InListPassiveTarget(1, nfc.BaudrateType_212kbitFeliCa, (byte*)&syscode, 2);
        // low-byte first service code.
        // Suica, Nimoca, etc. 0x090f system 0x0300
        // Edy service 0x170f (0x1317), system 0x00FE // 8280
        // FCF 1a8b
        len = nfc.felica_Polling(tmp, syscode);
        mon << "polling: " << len << endl;

        mon << "Request Service code and read blocks: " << endl;
        word scver = nfc.felica_RequestService(0x1a8b);
        mon.print(0x1a8b, HEX);
       mon << ": ";
       mon.print(scver, HEX);
       mon << endl;
        if ( scver ) {
        }
      } 
    } 
  }

}

void PN532_init() {

  mon << "Firmware version: ";
  byte r = 0;
  byte buf[4];
  for (int i = 0; i < 10 ; i++) {
    if ( nfc.GetFirmwareVersion() && (r = nfc.getCommandResponse((byte*)buf)) )
      break;
    delay(250);
  }
  if (! r ) {
    mon << "Couldn't find PN53x on Wire." << endl;
    while (1); // halt
  }

  Serial << "PN53x IC ver. " << (char) buf[0] << ", Firmware ver. " 
    << (byte) buf[1] << '.' << (byte) buf[2] << endl;

  Serial.println("SAMConfiguration");
  nfc.SAMConfiguration();

}



