#include "Wire.h"
#include "PN532_I2C.h"
#include "ISO14443.h"

//#include "CharacterLCD.h"
//#include "LCD_ST7032i.h"

#include <SPI.h>
#include "DS3234.h"

#include <string.h>

#include <TextStream.h>

DS3234 rtc(8);
long clockval;
//LCD_ST7032i lcd;

TextStream mon(Serial);
byte buf[48];

#define IRQ   (2)
#define RESET (0xff)  // Not connected by default on the NFC Shield
PN532 nfc(PN532::I2C_ADDRESS, IRQ, RESET);

const byte IizukaKey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const byte factory_a[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

long prev;
ISO14443 card;

byte polling[] = {
  2,
  TypeA,
  TypeF
};


void setup() {
  Serial.begin(57600);
  mon << "up." << endl;

  SPI.begin();
  rtc.begin();

  Wire.begin();
  nfc.begin();
//  lcd.begin();

  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  tone(4, 1760, 20);

  mon << "Hi." << endl;
  rtc.update();
  mon.printBytes((byte*) &rtc.cal, 3, HEX, '/');
  mon << " ";
  mon.printBytes((byte*) &rtc.time, 3, HEX, ':');
  mon<< endl;

//  lcd.setContrast(42);
//  lcd.print(rtc.calendarString((char*) buf));

  PN532_init();
  card.clear();

  delay(500);
}

void loop() {
  ISO14443 tcard;
  byte c;

  if ( millis() > prev + 50 ) {
    prev = millis();
    if ( (c = nfc.InAutoPoll(1, 1, polling+1, polling[0])) && (c = nfc.getAutoPollResponse((byte*) buf)) ) {
      //mon << mon.printArray(tmp, 8) << mon.endl;
      // NbTg, type1, length1, [Tg, ...]
      tcard.set(buf[1], buf+3);
      if ( tcard != card ) { 
        card = tcard;
        if ( card.type == 0x11 ) {
          rtc.update();
          Serial << rtc.cal << " " 
            << rtc.time << endl;
          //
          tone(4, 1800, 100);

          mon << "FeliCa IDm: ";
          mon.printBytes( card.IDm, 8);
//          mon << ", PMm: ";
//          mon.printBytes( card.PMm, 8);
          mon  << " detected. " << endl;

          if ( read_FCF() == 0 ) {
            mon << endl;
            tone(4, 1200, 100);
            card.clear();
          }
        } 
        else if ( card.type == 0x10 ) {
          rtc.update();
          mon << rtc.cal << " " 
            << rtc.time << endl;
          tone(4, 1800, 100);
          mon << "Mifare" << "  ID: ";
          mon.printBytes(card.UID, card.IDLength);
          mon << endl;
          nfc.targetSet(0x10, card.UID, card.IDLength);
          if ( nfc.mifare_AuthenticateBlock(4, IizukaKey_b) ) {
            mon << "Auth Success." << endl;
            nfc.mifare_ReadDataBlock(4, buf);
            mon.printBytes(buf, 16); 
            mon << "  ";
            mon.printBytes((char*) buf, 16); 
            mon << endl;
            nfc.mifare_ReadDataBlock(5, buf);
            mon.printBytes(buf, 16); 
            mon << "  ";
            mon.printBytes((char*)buf, 16); 
            mon << endl;
            nfc.mifare_ReadDataBlock(6, buf);
            mon.printBytes(buf, 16); 
            mon << "  ";
            mon.printBytes((char*)buf, 16); 
            mon << endl;
          } 
          else {
            tone(4, 1200, 100);
            Serial << "Failure." << endl;
          }
          Serial << endl;
        }
      } 
    }
    else {
      card.clear();
    }
  }
  //delay(250);
}

byte read_FCF() {
  word syscode = 0x00FE;
  int len;
  byte c;

  // Polling command, with system code request.
  len = nfc.felica_Polling(buf, syscode);
  if ( len == 0 )
    return 0;
  mon << "System code ";
  mon.printBytes((byte*)&syscode, 1);
  mon <<" specific IDm ";
  mon.printBytes(buf+1, 8);
  Serial << endl;
  // low-byte first service code.
  // Suica, Nimoca, etc. 0x090f system 0x0300
  // Edy service 0x170f (0x1317), system 0x00FE // 8280
  // FCF 1a8b
  mon << "Request Service code: ";
  word servcode = 0x1a8b;
  word scver = nfc.felica_RequestService(servcode);
  mon.print(servcode, HEX);
  mon << ": "; 
  mon.println(scver, HEX);

  if ( scver == 0xffff ) 
    return 0;
  word blist[] = { 0, 1, 2, 3};
    c = nfc.felica_ReadBlocksWithoutEncryption(buf, servcode, (byte) 1, blist);
    if ( c == 0 ) {
      Serial << endl << "failed. " << endl;
      return 0;
    }
    mon.printBytes(buf, 16);
    mon << "  ";
    mon.printBytes((char*) buf, 16);
    mon << endl;
  mon << "--- End of FCF reading ---" << endl << endl;
  return 1;
}


void PN532_init() {

  mon << "Firmware version: ";
  unsigned long r = 0;
  for (int i = 0; i < 10 ; i++) {
    if ( (r = nfc.GetFirmwareVersion()) )
      break;
    delay(250);
  }
  if (! r ) {
    mon << "Couldn't find PN53x on Wire." << endl;
    while (1); // halt
  } 
  else {
    mon.printBytes((byte*)&r, 4); 
    mon << endl;
  }

  // Got ok data, print it out!
  mon << "Found chip PN5";
  mon.println(r & 0xff, HEX);
  mon << "Firmware ver. " << (r>>8 & 0xFF) << '.' 
    << (r>>16 & 0xFF) << endl;

  mon << "SAMConfiguration" << endl;

  nfc.SAMConfiguration();
  mon << endl;
}
























