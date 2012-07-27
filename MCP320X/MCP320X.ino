#include <SPI.h>
#include "MCP320X.h"

MCP320X adc(10);
const int samplesize = 256;
void setup() {
  Serial.begin(19200);

  SPI.begin();
  adc.begin();

}

void loop() {
  int data[samplesize];
  int base;
  long sum = 0;
  float avr;
  int i;
  for(i = 0; i < samplesize; i++) {
    data[i] = adc.read12(adc.SINGLE_CH0);
    base = adc.read12(adc.SINGLE_CH1);
    data[i] -= base;
    delayMicroseconds(130);
  }
  for(i = 0; i < samplesize; i++) {
    sum += data[i]*data[i];
    /*
    for(i = -20; i <= data[i]; i++) {
      Serial.print('*');
    }
    */
    Serial.println(data[i]);
  }
  avr = sqrt(sum / samplesize);
  Serial.print("Vrms = ");
  Serial.println(avr, 2);
  Serial.println();
  delay(500);
}


