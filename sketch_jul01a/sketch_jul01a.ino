#include <Wire.h>
#include "PN532_I2C.h"
#include "IOExpander.h"
#include "MCP23009.h"
#include "CharacterLCD.h"
#include "xLCD.h"
//#include "LCD_ST7032i.h"

#include <SPI.h>
#include "DS3234.h"


#define IRQ   (2)
#define RESET (0xff)  // Not connected by default on the NFC Shield
PN532 nfc(PN532::I2C_ADDRESS, IRQ, RESET);

const byte cardkey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };

xLCD lcd(0x00, 1, 0xff, 2, 3, 4, 5, 6, 7); // for adafruit i2c/spi lcd adapter
//LCD_ST7032i lcd;

DS3234 rtc(8);
long clockval;
long prev;

struct ID {
  byte value[8];
  byte length;

  ID() {
  }

  ID(const byte * val, const byte len) : 
  length(len) {
    memcpy(value, val, length);
  }

  ID(const ID & id) {
    length = id.length;
    memcpy(value, id.value, id.length);
  }

  void set(const byte * val, const byte len) {
    length = len;
    memcpy(value, val, length);
  }

  boolean operator==(const ID & another) {
    if ( length != another.length )
      return false;
    return memcmp(value, another.value, length) == 0;
  }

  inline boolean operator!=(const ID & another) {
    return !(*this == another);
  }
} 
id;

void setup() {
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  Serial.begin(19200);
  Serial.println("yes.");

  SPI.begin();
  rtc.begin();
  rtc.updateTime();
  clockval = rtc.time;

  Wire.begin();
  nfc.begin();
  lcd.begin(16, 2);

  lcd.backlightOn();

  unsigned long r = nfc.getFirmwareVersion();
  Serial.println();
  Serial.println(r, HEX);
  if (! r ) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  } 

  // Got ok data, print it out!
  Serial.print("Found chip PN5"); 
  Serial.println(r & 0xff, HEX);
  Serial.print("Firmware ver. "); 
  Serial.print(r>>8 & 0xFF, DEC); 
  Serial.print('.'); 
  Serial.println(r>>16 & 0xFF, DEC);

  lcd.clear();
  delay(100);
  lcd.print("PN5");
  lcd.print(r & 0XFF, HEX);
  lcd.print(" Firm V.");
  lcd.print(r>>8 & 0xFF, DEC);
  lcd.print('.'); 
  lcd.print(r>>16 & 0xFF, DEC);

  Serial.println("SAMConfiguration");
  nfc.SAMConfiguration();

  lcd.setCursor(0,1);
  lcd.print("SAM Config.");
  delay(1000);
  lcd.clear();
  delay(1000);

  id.length = 0;
}

void loop() {
  byte c, tmp[64];
  byte buf[32];
  ID card;

  if ( millis() > prev + 50 ) {
    prev = millis();
    rtc.updateTime();
    if ( clockval != rtc.time ) {
      clockval = rtc.time;
      lcd.setCursor(0,0);
      lcd.print(rtc.timeString((char*)tmp));
    }
    if ( (c = nfc.listPassiveTarget(tmp)) != 0 ) {
      card.set(tmp+6, tmp[5]);
      if ( card != id ) {
        id = card;
        Serial.print(rtc.timeString((char*)buf));
        Serial.println(" Mifare");
        Serial.print("Card UID bytes: ");
        for(int i = 0; i < id.length; i++) {
          Serial.print(id.value[i]>>4&0x0f,HEX);
          Serial.print(id.value[i]&0x0f,HEX);
        }
        Serial.println();

        //        lcd.clear();
        lcd.setCursor(10,0);
        lcd.print("Mifare");
        lcd.setCursor(0,1);
        for(int i = 0; i < id.length; i++) {
          lcd.print(id.value[i]>>4 & 0x0f, HEX);
          lcd.print(id.value[i] & 0x0f, HEX);
        }

        if ( !nfc.mifare_AuthenticateBlock((uint8_t*)tmp+6, tmp[5], 4, 0, (uint8_t*)cardkey_b+1) ) {
          Serial.println("Authenticated.");
        }
        digitalWrite(5, LOW);
        digitalWrite(5, HIGH);
        if ( nfc.mifare_ReadDataBlock(4,tmp) ) {
          /*
    Serial.println("Block contents: ");
           nfc.printHexString(tmp, 24);
           Serial.println();
           */
          for(int i = 0; i < 12; i++) {
            Serial.print((char)tmp[i]);
          }
          Serial.println();

          lcd.setCursor(0,1);
          for(int i = 0; i < 12; i++) {
            lcd.print((char) tmp[i]);
          }
        }
      }
    } 
    else 
      if ( (c = nfc.listPassiveTarget(tmp,nfc.BaudrateType_212kbitFeliCa)) != 0 ) {
      card.set(tmp+4, 8);
      if ( card != id ) {
        id = card;
        Serial.print(rtc.timeString((char*)buf));
        Serial.println(" Felica");
        Serial.print("Card UID bytes: ");
        for(int i = 0; i < id.length; i++) {
          Serial.print(id.value[i]>>4&0x0f,HEX);
          Serial.print(id.value[i]&0x0f,HEX);
        }
        Serial.println();

        Serial.print("POL_RES length = ");
        Serial.println(tmp[2], HEX);
        Serial.print("Resp. code = ");
        Serial.println(tmp[3], HEX);
        Serial.println("Manufacturer bytes: ");
        nfc.printHexString(tmp+12,8);
        Serial.println();

        //        lcd.clear();
        lcd.setCursor(10,0);
        lcd.print("FeliCa");
        lcd.setCursor(0,1);
        for(int i = 0; i < 8; i++) {
          lcd.print(tmp[4+i]>>4 & 0x0f, HEX);
          lcd.print(tmp[4+i] & 0x0f, HEX);
        }
      }
    } 
    else {
      id.length = 0;
    }
  }
  delay(100);
}

















