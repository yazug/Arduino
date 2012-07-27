/*
 *
 *  ^CS |1   20| SCLK  -> D13 (PB5/SCLK)
 *  NC  |    19| DOUT  -> D12 (PB4/MISO)
 * 32kHz|    18| SCLK
 *  Vcc |4   17| DIN   -> D11 (PB3/MOSI)
 *  ^INT|    16| Vbat
 *  ^RST|    15| GND
 *
 */

#include <SPI.h>
#include "DS3234.h"
#include <Wire.h>
#include "CharacterLCD.h"
//#include "IOExpander.h"
#include "MCP23009.h"
#include "xLCD.h"

#include <string.h>

unsigned long atoBCD(char * str) {
  unsigned long val = 0;
}

unsigned long asBCD(unsigned long lval) {
  unsigned long tmp;
  byte * p = (byte*)(&tmp);
  int i;
  for ( i = 0; i < 4; i++) {
    p[i] = lval%10;
    lval /= 10;
    p[i] |= (lval%10)<<4;
    lval /= 10;
  }
  return tmp;
}

byte decToBcd(byte d) { 
  return ((d/10)<<4) + (d%10); 
}	


DS3234 rtc(8);
long clockval;
xLCD lcd(0x00, 1, 0xff, 2, 3, 4, 5, 6, 7);

void setup() {

  Serial.begin(9600);
  SPI.begin();
  rtc.begin();
  
  /*
  byte a[] = {1};
   rtc.transfer(0x93, a,1);
   */
   
   Wire.begin();
   lcd.begin();
   lcd.clear();
   lcd.backlightOn();
   
  delay(500);
}

void loop() {
  long temp;
  byte buf[16], * p;
  byte c, serbuf[32];
  long idlemillis;
  int bp;

  if (Serial.available()) {
    bp = 0;
    while (Serial.available()) {
      idlemillis = millis();
      c = Serial.read();
      serbuf[bp] = c;
      bp++;
      bp %= 32;
      serbuf[bp] = 0;
      while (!Serial.available()) {
        if (millis() - idlemillis > 500)
          break;
      }
      if ( millis() - idlemillis > 500 
        || c == '.' ) {
        break;
      }
    }

    if ( c == '.' ) {
      switch(serbuf[0]) {
      case 'T':
      case 't':
        temp = atol((char*)&serbuf[1]);
        temp = asBCD(temp);
        rtc.setTime(temp);
        lcd.setCursor(0,1);
        lcd.print("Set time done.");
        break;
      case 'C':
      case 'c':
        temp = atol((char*)&serbuf[1]);
        temp = asBCD(temp);
        rtc.setCalendar(temp);
        lcd.setCursor(0,1);
        lcd.print("Set calendar.");
        break;
      }
    }
  }

  delay(50);
  rtc.updateTime();
  if ( clockval != rtc.time ) {
    clockval = rtc.time;
    rtc.updateCalendar();

    Serial.print(rtc.calendarString((char*)buf));
    Serial.print(" ");
    Serial.print(rtc.timeString((char*)buf));
    Serial.print(" ");
    lcd.setCursor(0,0);
    lcd.print((char*)buf);
    Serial.print(rtc.copyNameOfDay((char*)buf, rtc.dayOfWeek()));
    Serial.println();
    lcd.setCursor(10,0);
    lcd.print((char*)buf);

    if ( (rtc.time & 0xff) == 0 || (rtc.time & 0xff) == 0x30 ) {
      rtc.updateTemperature();
      Serial.print(rtc.tempt/100.0);
      Serial.println(" C.");
      lcd.setCursor(0,1);
      lcd.print(rtc.tempt/100.0);
      lcd.print("C\0x81");
    }
    delay(50);
  }
}



