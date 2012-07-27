#include <SPI.h>
#include "MCP48X2.h"

const byte cs = 8;
MCP48X2 dac(cs);
word w;

void setup() {
//  Serial.begin(19200);
    
  SPI.begin();
  dac.begin();
  dac.setupSPI();
  
  w = 0;  
}

void loop() {
  //word w = millis() >> 8;
  //w ^= 0xfff;
  w+= 64;

  dac.write12(w);
//  Serial.println(w & 0xfff, HEX);
  
  delayMicroseconds(5);
}

