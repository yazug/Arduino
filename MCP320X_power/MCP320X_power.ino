#include <SPI.h>
#include "MCP320X.h"

MCP320X adc(10);

void setup() {
  Serial.begin(38400);

  SPI.begin();
  adc.begin();

}

void loop() {
  int data[128];
  long sum = 0;
  long avr;
  int i;
  for(i = 0; i < 128; i++) {
    data[i] = adc.read12(adc.SINGLE_CH0);
    sum += data[i];
    delayMicroseconds(667);
  }
  avr = sum / 128;
  for(i = 0; i < 128; i++) {
    for(int c = -20; c <= data[i] - avr; c++) {
      Serial.print('*');
    }
    Serial.println();
  }
  delay(1000);
}


