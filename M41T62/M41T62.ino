#include <Wire.h>
#include "M41T62.h"

M41T62 rtc;

void setup(void) {
  Serial.begin(19200);
}

void loop(void) {
  byte tmp[32];
  rtc.readRegisters(0x00, tmp, 4);
  Serial.print(tmp[0], HEX);
  Serial.print(' ');
  Serial.print(tmp[1], HEX);
  Serial.print(' ');
  Serial.print(tmp[2], HEX);
  Serial.print(' ');
  Serial.print(tmp[3], HEX);
  Serial.print(' ');
  Serial.println(rtc.time, HEX);
  delay(1000);
}


