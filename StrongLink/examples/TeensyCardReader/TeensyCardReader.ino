/**
 *  @title:  StrongLink SL018/SL030 RFID reader demo
 *  @author: marc@marcboon.com
 *  @see:    http://www.stronglink.cn/english/sl018.htm
 *  @see:    http://www.stronglink.cn/english/sl030.htm
 *
 *  Arduino to SL018/SL030 wiring:
 *  A4/SDA     2     3
 *  A5/SCL     3     4
 *  5V         4     -
 *  GND        5     6
 *  3V3        -     1
 */
#include <string.h>

#include <Wire.h>
#include <DS1307.h>
#include <StrongLinkI2C.h>
#include <SPI.h>
#include "SDCard.h"

/*** Mifare Card Reader ***/

const byte pin_tagsta = 9;
StrongLinkI2C rfid(StrongLinkI2C::SL018_ADDRESS, pin_tagsta);
StrongLinkI2C::CardInfo lastcard;

const byte cardkey_a[] = {
  0xAA, 0x4B, 0x61, 0x5A, 0x75, 0x49, 0x69 };
const byte cardkey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const byte philips_key[] = { 
  0xAA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

/*** SD Card RW ***/
const byte pin_SD = 10;
SDCard sd(pin_SD);

/*** RTC ***/
DS1307 rtc;

/*** Input buttons ***/
const byte pin_button_b = 13;
const byte pin_button_w = 12;
boolean button_b_down;
const byte pin_LED = 11;

void setup() {
  pinMode(pin_button_b, INPUT);
  digitalWrite(pin_button_b, HIGH);
  button_b_down = false;
  pinMode(pin_LED, OUTPUT);
  pinMode(pin_SD, OUTPUT);
  digitalWrite(pin_SD, HIGH);

  Wire.begin();
  rtc.begin();
  //  Serial.begin(19200);
  SPI.begin();

  if ( !sd.begin() ) {
    //    Serial.println("SD Card mount failed.");
    Keyboard.println("SD card mount failed.");
  }
  long clock, cal;
  char buf[32];
  rtc.getTime(clock);
  rtc.getCalendar(cal);
  File dataFile = sd.open("cardlog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print("# ");
    dataFile.print( rtc.calendarString(buf, clock) );
    dataFile.print(" ");
    dataFile.print( rtc.clockString(buf, clock) );
    dataFile.print(" reader firmware ver ");
    dataFile.println( rfid.firmware_version()/10.0 );
    dataFile.close();
  }
  // prompt for tag
  //Serial.println("Show me your tag");
}

void loop() {
  int i;
  long time, cal;
  char buf[64];

  // start seek mode
  if ( rfid.detect() && rfid.select() ) {
    if ( !rfid.identical(lastcard) ) {
      digitalWrite(pin_LED, HIGH);
      rfid.get_card_info(lastcard);
      /*
      for ( i = 0; i < lastcard.uid_length; i++) {
       Serial.print(lastcard.uid[i]>>4&0x0f, HEX);
       Serial.print(lastcard.uid[i]&0x0f, HEX);
       Serial.print(" ");
       }
       Serial.print(" type: ");
       Serial.println(rfid.get_type_name(buf, lastcard.type));
       */
      if ( rfid.login_sector(1, cardkey_b) 
        && rfid.read_block((1<<2), (byte *) buf) ) {
        tone(20, 3526, 200);
        tone(20, 4186, 100);
        buf[2+8] = 0;
        long sid = atol((const char*) (buf+2));
        /*
        if ( lastcard.uid_length > 4 ) {
         Serial.print(*((long *)(lastcard.uid+4)), HEX);
         Serial.print('.');
         }
         */
        rtc.getTime(time);
        rtc.getCalendar(cal);
        Keyboard.print( rtc.clockString(buf, time) );
        Keyboard.set_key1(KEY_TAB);
        Keyboard.send_now();
        Keyboard.set_key1(0);
        Keyboard.send_now();
        Keyboard.print( rtc.calendarString(buf, cal) );
        Keyboard.set_key1(KEY_TAB);
        Keyboard.send_now();
        Keyboard.set_key1(0);
        Keyboard.send_now();
        Keyboard.print(sid);
        Keyboard.println();

        File dataFile = sd.open("cardlog.txt", FILE_WRITE);
        if (dataFile) {
          dataFile.print( rtc.clockString(buf, time) );
          dataFile.print("\t");
          dataFile.print( rtc.calendarString(buf, cal) );
          dataFile.print("\t");
          dataFile.print(sid);
          dataFile.print("\t");
          dataFile.print(*((long *)(lastcard.uid)), HEX);
          dataFile.println();
          dataFile.close();
        }
      } 
      else {
        Serial.println("login sector or/and read block failed.");
      }
      digitalWrite(pin_LED, LOW);
    }
  } 
  else if ( lastcard.uid_length != 0 ) {
    memset(&lastcard, 0, StrongLinkI2C::CARDINFO_LENGTH);
    Serial.println("Connection Lost.");
    delay(500);
  } 
  if ( !button_b_down && digitalRead(pin_button_b) == LOW ) {
    button_b_down = true;
  } 
  else if ( button_b_down && digitalRead(pin_button_b) == HIGH ) {
    digitalWrite(pin_LED, HIGH);
    button_b_down = false;
    rtc.getTime(time);
    rtc.getCalendar(cal);
    Keyboard.print(rtc.calendarString(buf, cal));
    Keyboard.print(" ");
    Keyboard.print(rtc.clockString(buf, time));
    Keyboard.println(". Ready.");
    digitalWrite(pin_LED, LOW);
  } 
  else {
    delay(50);
  }
  //  }
}













