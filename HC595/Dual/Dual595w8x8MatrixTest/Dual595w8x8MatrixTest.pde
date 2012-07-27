#include <SPI.h>
#include "matrix8x8.h"
//#include "TimerCounter.h"
#include "IOExpander.h"
#include "HC595iox.h"

const int latchPin = 10;   //Pin connected to ST_CP of 74HC595
const int clockPin = 13;   //Pin connected to SH_CP of 74HC595
const int dataPin  = 11;   //Pin connected to DS of 74HC595

unsigned char message[16];

HC595iox xpander(latchPin);
int r = 0;
long elapsed;
long stepwait = 0;
int offset = 0;

void setup() {
  SPI.begin();
  xpander.begin();
  analogWrite(3,30); 
  strcpy((char*)message, "   Not Happy   ");
  elapsed = micros();
}

void loop() {
  word bmap[2];
  stepwait++;
  if ( stepwait > 7000 ) {
    stepwait = 0;
    offset++;
    offset %= (fontWidth+1)*(strlen((char*)message)-3);
  }
  if ( micros() - elapsed > 433 ) {
    r = (r+1) % 8;
    bmap[0] = allColumnOff;
    bmap[1] = allColumnOff;
    bitSet(bmap[0], row[r]);
    bitSet(bmap[1], row[r]);
    for(int c = 0; c < 16; c++) {
      int ch = message[(c+offset)/(fontWidth+1)] - 0x20;
      if ( (c+offset) % (fontWidth+1) != fontWidth ) {
        if ( bitRead(font[ch][(c+offset)%(fontWidth+1)], r) ) {
          bitClear(bmap[c/8], col[c%8]);
        }
      }
    }
    xpander.select();
    xpander.transfer(highByte(bmap[0]));
    xpander.transfer(lowByte(bmap[0]));
    xpander.transfer(highByte(bmap[1]));
    xpander.transfer(lowByte(bmap[1]));
    xpander.deselect();
    //
    elapsed = micros();
  }
} 



