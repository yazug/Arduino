#include <Wire.h>
#include "M41T62.h"

#include <string.h>

M41T62 rtc;
long clockval;

void setup() {
//  pinMode(7, INPUT);
//  digitalWrite(7, HIGH);
  
  Serial.begin(19200);
  Serial.println("Hi.");
  
  Wire.begin();
  rtc.begin();
  /*
  byte a[] = {1};
   rtc.transfer(0x93, a,1);
   */

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
        temp = strtol((char*)&serbuf[1], NULL, 16);
        rtc.setTime(temp);
        Serial.println(temp, HEX);
        break;
      case 'C':
      case 'c':
        temp = strtol((char*)&serbuf[1], NULL, HEX);
        rtc.setCalendar(temp);
        Serial.println(temp, HEX);
        break;
      }
    }
  }

  delay(100);
  rtc.updateTime();
  if ( rtc.time != clockval ) {
    clockval = rtc.time;
    rtc.updateCalendar();
    Serial.print( rtc.time, HEX );
    Serial.print(" ");
    Serial.print( rtc.cal, HEX);
    Serial.print(" ");
    Serial.print( rtc.copyNameOfDay((char*) buf, rtc.dayOfWeek()) );
    Serial.println();
    delay(100);
  }
}




