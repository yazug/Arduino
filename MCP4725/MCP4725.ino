#include  <Wire.h>
#include "MCP4725.h"

MCP4725 dac;

void setup()
{
  Wire.begin();
  dac.begin();
}


void loop()
{
  for(int i = 0; i < 256; i ++) {
    dac.write(abs(256-i));
  }
  delayMicroseconds(100);
}

