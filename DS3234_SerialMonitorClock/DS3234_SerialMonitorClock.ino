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


DS3234 rtc(9);
long clockval;

void setup() {

  byte not_used_cs[] = { 4, 5, 10 };
  for (int i = 0; i < sizeof(not_used_cs); i++) {
    pinMode(not_used_cs[i], OUTPUT);
    digitalWrite(not_used_cs[i], HIGH);
  }

  Serial.begin(9600);
  Serial.println("Hi.");
  Serial.println();

  SPI.begin();
  rtc.begin();
  delay(100);
  Serial.println("CONTROL register ");
  for (int bp = 7; bp >= 0; bp--)
    Serial.print(rtc.readRegister(DS3234::REGISTER_CONTROL)>>bp & 1, BIN);
  Serial.println("\n\nCONTROL_STATUS register");
  for (int bp = 7; bp >= 0; bp--)
    Serial.print(rtc.readRegister(DS3234::REGISTER_CONTROL_STATUS)>>bp & 1, BIN);
  Serial.println();
  Serial.println();
  delay(100);
}

void loop() {
  long temp;
  byte * p;
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
        rtc.time = strtol((char*)&serbuf[1], NULL, 16);
        rtc.setTime();
        Serial.println(rtc.time, HEX);
        break;
      case 'C':
      case 'c':
        rtc.cal = strtol((char*)&serbuf[1], NULL, HEX);
        rtc.setCalendar();
        Serial.println(rtc.cal, HEX);
        break;
      }
    }
  }

  delay(100);
  rtc.updateTime();
  if ( clockval != rtc.time ) {
    clockval = rtc.time;
    rtc.updateCalendar();

    Serial.print("20");
    Serial.print(rtc.cal>>20&0x0f, HEX);
    Serial.print(rtc.cal>>16&0x0f, HEX);
    Serial.print('/');
    Serial.print(rtc.cal>>8&0xff, HEX);
    Serial.print('/');
    Serial.print(rtc.cal&0xff, HEX);
    Serial.print(' ');
    Serial.print(rtc.time>>20&0x0f, HEX);
    Serial.print(rtc.time>>16&0x0f, HEX);
    Serial.print(':');
    Serial.print(rtc.time>>12&0x0f, HEX);
    Serial.print(rtc.time>>8&0x0f, HEX);
    Serial.print(':');
    Serial.print(rtc.time>>4&0x0f, HEX);
    Serial.print(rtc.time&0x0f, HEX);
    Serial.print(' ');
    Serial.print(rtc.copyNameOfDay((char*)serbuf, rtc.dayOfWeek()));
    Serial.print(' ');
    Serial.print(rtc.copyNameOfMonth((char*)serbuf, rtc.cal>>8&0x0f));
    Serial.println();

    if ( (rtc.time & 0xff) == 0 || (rtc.time & 0xff) == 0x30 ) {
      rtc.updateTemperature();
      Serial.print(rtc.tempt/100.0);
      Serial.println(" C.");
    }
    delay(100);
  }
}





