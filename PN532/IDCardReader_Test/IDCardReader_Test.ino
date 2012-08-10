#include "Wire.h"
#include "PN532_Wire.h"
#include "ISO14443.h"

#include "CharacterLCD.h"
#include "LCD_ST7032i.h"

#include <SPI.h>
#include "DS3234.h"

#include <string.h>

#include <Monitor.h>

DS3234 rtc(8);
long clockval;
LCD_ST7032i lcd;

Monitor mon(Serial);
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
  lcd.begin();

  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  tone(4, 1760, 20);

  mon << "Hi." << endl;
  rtc.update();
  mon << rtc.calendarString((char*) buf) << " " << rtc.timeString((char*) buf) << endl;
  lcd.setContrast(42);
  lcd.print(rtc.calendarString((char*) buf));

  PN532_init();
  card.init();

  delay(500);
}

void loop() {
  ISO14443 tcard;
  byte c;

  if ( millis() > prev + 50 ) {
    prev = millis();
    if ( (c = nfc.autoPoll(polling, buf)) ) {
      //mon << mon.printArray(tmp, 8) << mon.endl;
      // NbTg, type1, length1, [Tg, ...]
      tcard.set(buf[1], buf+3);
      if ( tcard != card ) { 
        card = tcard;
        if ( card.type == 0x11 ) {
          rtc.update();
          Serial << rtc.calendarString((char*) buf) << " " 
            << rtc.timeString((char*) buf) << endl;
          //
          tone(4, 1800, 100);

          mon << "FeliCa IDm: ";
          mon.printHex( card.IDm, 8);
          mon << ", PMm: ";
          mon.printHex( card.PMm, 8);
          mon  << " detected. " << endl;

          if ( read_FCF() == 0 ) {
            mon << endl;
            tone(4, 1200, 100);
            card.init();
          }
        } 
        else if ( card.type == 0x10 ) {
          rtc.update();
          mon << rtc.calendarString((char*) buf) << " " 
            << rtc.timeString((char*) buf) << endl;
          tone(4, 1800, 100);
          mon << "Mifare" << "  ID: ";
          mon.printHex(card.UID, card.IDLength);
          mon << endl;
          if ( nfc.mifare_AuthenticateBlock(card.UID, card.IDLength, 4,
          IizukaKey_b) ) {
            mon << "Auth Success." << endl;
            nfc.mifare_ReadDataBlock(4, buf);
            mon.printHex(buf, 16); 
            mon << "  ";
            mon.printHex((char*) buf, 16); 
            mon << endl;
            nfc.mifare_ReadDataBlock(5, buf);
            mon.printHex(buf, 16); 
            mon << "  ";
            mon.printHex((char*)buf, 16); 
            mon << endl;
            nfc.mifare_ReadDataBlock(6, buf);
            mon.printHex(buf, 16); 
            mon << "  ";
            mon.printHex((char*)buf, 16); 
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
      card.init();
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
  mon.printHex(&syscode, 1);
  mon <<" specific IDm ";
  mon.printHex(buf, 8);
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

  for (word blkno = 0; blkno < 4; blkno++) {
    c = nfc.felica_ReadWithoutEncryption(buf, servcode, 1, (word*)&blkno);
    if ( c == 0 ) {
      Serial << endl << "failed. " << endl;
      return 0;
    }
    mon.printHex(&blkno, 1) ;
    mon << ": ";
    mon.printHex(buf, 16);
    mon << "  ";
    mon.printHex((char*) buf, 16);
    mon << endl;
  }
  mon << "--- End of FCF reading ---" << endl << endl;
  return 1;
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
    mon.printHex((byte*)&r, 4); 
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























