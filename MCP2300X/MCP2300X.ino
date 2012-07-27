#include <Wire.h>

#include "IOExpander.h"
#include "MCP23009.h"

MCP23009 iox(0);
boolean t = false;
void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(1000);
  
  iox.init();
  iox.pinMode(0, OUTPUT);
  iox.pinMode(1, OUTPUT);
  iox.pullup(0xff);
  Serial.println("iox initialized.");
}

void loop() {
//  byte t = iox.read();
//  Serial.println(t, HEX);
  iox.digitalWrite(0, t);
  Serial.println(iox.digitalRead(1), DEC);
  t = ~t;
  delay(100);
}

