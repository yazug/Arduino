//#include <DataFlash.h>

#include "NewSoftSerial.h"

const byte soft_rx = 3;
const byte soft_tx = 2;
NewSoftSerial serport(soft_rx, soft_tx);
const byte uALFAT_reset = 4;

void setup() {
  word year = 2010;
  word month = 8;
  word date = 9;
  word hour = 23;
  word minutes = 35;
  word seconds = 30;

  unsigned long dword;

  dword = (year - 1980)<<25;
  dword |= (month & 0x0f)<<21;
  dword |= (date & 0x1f)<< 16;
  dword |= (hour & 0x1f)<<11;
  dword |= (minutes & 0x3f)<<5;
  dword |= ((seconds>>1) & 0x1f);

  Serial.begin(9600);
  serport.begin(9600);
  pinMode(uALFAT_reset, OUTPUT);
  digitalWrite(uALFAT_reset, LOW); 
  digitalWrite(uALFAT_reset, HIGH); 
}

char buff[64];
byte ibegin = 0, iend = 0;
boolean escape = false;
void loop() {
  char c;
  while (serport.available()) {
    c = serport.read();
    if (c == 0x0d) 
      c = '\n';
    buff[iend++] = c;
    iend %= 64;
    if ( c == '\n') {
      while ( ibegin != iend ) {
        Serial.print(buff[ibegin++]);
        ibegin %= 64;
      }
    }
  }

  while (Serial.available()) {
    c = Serial.read();
    if ( escape ) {
      if (c == 'r') {
        c = 0x0d;
      }
      escape = false;
    } 
    else {
      switch (c) {
      case '¥':
      case '\\':
        escape = true;
        continue;
        break;
      case '?':
        c = 0x0d;
        break;
      }
    }
    serport.print((char)c);
    //    Serial.print((char)c);
  }
}

