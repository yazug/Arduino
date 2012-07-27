#include <SPI.h>
#include "DS3234.h"
#include "SerialMonitor.h"

char DAYS[][4] = {
  "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

char MONTHS[][4] = {
  "Dec", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

byte decToBcd(byte d) { 
  return ((d/10)<<4) + (d%10); 
}	

DS3234 rtc(10);
long clockval;

void setup() {
  byte buf[8];

  Serial.begin(9600);
  pinMode(9,OUTPUT);
  digitalWrite(9,HIGH);
  SPI.begin();
  rtc.init();
  rtc.transfer((byte)0x11, buf, 2);
  Serial.print(buf[0], BIN);
  Serial.print(" ");
  Serial.print(buf[1], BIN);
  Serial.println();
  delay(1000);
}

void loop() {
  long temp;
  byte buf[8], * p;
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
        temp = SerialMonitor::asBCD(temp);
        rtc.setTime(temp);
        break;
      case 'C':
      case 'c':
        temp = atol((char*)&serbuf[1]);
        temp = SerialMonitor::asBCD(temp);
        rtc.setCalender(temp);
        break;
      }
    }
    return;
  }

  rtc.getTime(temp);
  if ( temp != clockval ) {
    clockval = temp;
    Serial.print(((byte*)&clockval)[2], HEX);
    Serial.print(":");
    Serial.print(((byte*)&clockval)[1], HEX);
    Serial.print(":");
    Serial.print(((byte*)&clockval)[0], HEX);
    Serial.print(" ");
    rtc.getCalender(temp);
    Serial.print("20");
    Serial.print(((byte*)&temp)[3]>>4, HEX);
    Serial.print(((byte*)&temp)[3]&0x0f, HEX);
    Serial.print(" ");
    Serial.print(MONTHS[((byte*)&temp)[2]]);
    Serial.print(" ");
    Serial.print(((byte*)&temp)[1], HEX);
    Serial.print(" ");
    Serial.print(DAYS[temp&0xff]);
    Serial.print("  ");

    //    Serial.print((char*)rtc.getTimestamp(serbuf));
    /*
    rtc.transfer(0x11, buf, 2);
     temp = (buf[0]<<8) | buf[1];
     temp >>= 6;
     */
    int t;
    rtc.getTemperature(t);
    //    Serial.print((float)temp/4, 2);
    Serial.print(t/10.0);
    Serial.print(" C.  ");

    Serial.println();
  }
  /* 
   rtc.transfer(0x00, buf, 3);
   Serial.print(buf[2], HEX);
   Serial.print(" ");
   Serial.print(buf[1], HEX);
   Serial.print(" ");
   Serial.print(buf[0], HEX);
   Serial.print(" ");
   rtc.transfer(0x11, buf, 2);
   temp = (buf[0]<<8) | buf[1];
   temp >>= 6;
   Serial.println(temp, HEX);
   */
  delay(100);
}


