#include <Wire.h>
#include "M41T62.h"

M41T62 rtc;

void setup(void) {
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  Wire.begin();
  rtc.begin();
  Serial.begin(19200);
}

void loop(void) {
  byte tmp[32];
  rtc.readRegisters(0x00, tmp, 8);
  for(int i = 0; i < 8; i++) {
    Serial.print(tmp[i], HEX);
    Serial.print(' ');
  }
  Serial.flush();
  Serial.println(rtc.time, HEX);
  Serial.flush();
  delay(1000);
}


