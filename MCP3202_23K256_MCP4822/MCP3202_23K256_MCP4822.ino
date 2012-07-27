#include <SPI.h>
#include "MCP320X.h"
#include "SPISRAM.h"
#include "MCP48X2.h"

MCP320X adc(10);
/*
  SO pin of MCP3202 is in Z state with pull-up
 while the 3 leading don't-care bits of returning value 
 */

const byte SRAM_CS = 9;
SPISRAM sram(SRAM_CS);
const byte DAC_CS = 8;
MCP48X2 dac(DAC_CS);

const word SAMPLE_SIZE = 32768;
void setup() {
  pinMode(7, OUTPUT);
//  Serial.begin(38400);

  SPI.begin();
  adc.begin();
  sram.begin();
  dac.begin();
}

void loop() {
  union {
    byte b[2];
    word w;
  } 
  val;
  long intime, outtime;
  word i;

digitalWrite(7, HIGH);
  intime = micros();
  for(i = 0; i < SAMPLE_SIZE; ) {
    adc.setupSPI();
    val.w = adc.read8(adc.SINGLE_CH0);
    sram.setupSPI();
//    sram.write(i, (byte*)&val, 2);
    sram[i++] = (byte) val.w;
//    i += 2;
    delayMicroseconds(32);
  }
  outtime = micros();
digitalWrite(7, LOW);

  delay(2000);
  sram.setupSPI();
  for(i = 0; i < SAMPLE_SIZE; ) {
    /*
    Serial.print(i);
     Serial.print(": ");
     */
     val.w = sram[i++];
//    val.b[0] = sram[i++];
    //    val <<=8;
//    val.b[1] = (sram[i++]);
    /*
    Serial.print(val.w, HEX);
     Serial.print(" ");
     Serial.print(val.w);
     Serial.println();
     */
    dac.write12(val.w<<4);
    delayMicroseconds(51);
  }
  /*
  Serial.print(SAMPLE_SIZE);
  Serial.print(" sample / ");
  Serial.print(outtime - intime);
  Serial.print(" usec, ");
  Serial.print(1000L * SAMPLE_SIZE/(outtime - intime));
  Serial.println(" ksps.");
  */
  delay(2000);
}






