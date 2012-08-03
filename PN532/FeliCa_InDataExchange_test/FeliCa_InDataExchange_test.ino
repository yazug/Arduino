#include "Wire.h"
#include "PN532_I2C.h"
#include "ISO14443.h"

#include "Monitor.h"

#define IRQ   (2)
#define RESET (0xff)  // Not connected by default on the NFC Shield
PN532 nfc(PN532::I2C_ADDRESS, IRQ, RESET);

Monitor mon(Serial);

long prev;
ISO14443 card;

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
  ISO14443 tcard;
  byte c, tmp[64];

  if ( millis() <= prev + 1000 ) 
    return;
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
        word syscode = 0x00FE;
        nfc.listPassiveTarget(tmp, nfc.BaudrateType_212kbitFeliCa, syscode);
        // low-byte first service code.
        // Suica, Nimoca, etc. 0x090f system 0x0300
        // Edy service 0x170f (0x1317), system 0x00FE // 8280
        // FCF 1a8b
        len = nfc.felica_Polling(tmp, syscode);
        mon << "polling: " << len << mon.endl;

        mon << "Request Service code and read blocks: " << mon.endl;
        word scver = nfc.felica_XRequestService(tmp,0x1a8b);
        mon << mon.printHexString(0x1a8b) << ": " 
          << mon.print(scver, HEX) << mon.endl;
        if ( scver ) {
        }
      } 
    } 
  }

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



