#include <SPI.h>
#include "MCP320X.h"
#include "SPISRAM.h"
#include "MCP48X2.h"

MCP320X adc(10);
/*
  SO pin of MCP3202 is in Z state with pull-up
 while the 3 leading don't-care bits of returning value 
 */

const byte DAC_CS = 8;
MCP48X2 dac(DAC_CS);

void setup() {
  pinMode(7, OUTPUT);
  //  Serial.begin(38400);

  SPI.begin();
  adc.begin(SPI_CLOCK_DIV16);
  dac.begin();
}

void loop() {
  union {
    byte b[2];
    word w;
  } 
  val;
  long intime, outtime;
  int i;

  for(;;) {
    adc.setupSPI();
    val.w = adc.read12(adc.SINGLE_CH0);
    dac.setupSPI();
    dac.write12(val.w);
//    delayMicroseconds(5);
  }
}








