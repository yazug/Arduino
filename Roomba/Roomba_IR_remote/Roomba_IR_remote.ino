#include <Roomba500.h>

const int IR_GND = 14;
const int IR_OUT = 15;
const int IR_VCC = 16;

inline byte IR_read() { 
  //return digitalRead(IR_OUT);
  return (PINC & (1<<1)) != 0; 
}

int duration_low, duration_high;
int last_falling, last_rising;

void setup() {
  pinMode(IR_GND, OUTPUT);
  digitalWrite(IR_GND, LOW);
  pinMode(IR_OUT, INPUT);
  digitalWrite(IR_OUT, HIGH);
  pinMode(IR_VCC, OUTPUT);
  digitalWrite(IR_VCC, HIGH);

  Serial.begin(19200);
  Serial.println();
  Serial.println("Start.");

  // init
  duration_low = 0;
  duration_high = 0;
  last_falling = 0;
  last_rising = 0;
}

void loop() {
  byte b = 0;
  if ( IR_read() == HIGH ) {
    long   qmilli = micros()>>8; // the last rising 
    long duration;
    while ( IR_read() == HIGH ) {
      duration = (micros()>>8) - qmilli;
      if ( duration > 39 ) {
        break;
      }
    }
    if ( IR_read() == HIGH ) {
      // if still high
      byte b = read_IR();
      if ( b == Roomba500::IR_VirtualWall ) {
        Serial.println("Virtual Wall");
      } else if ( b ) {
        Serial.println(b, DEC);
      }
    }
  }
}

byte read_IR() {
  byte value = 0;
  int bits = 0;
  long lastRising = micros()>>8;
  long lastFalling;
  int dhigh, dlow;
  if ( IR_read() != HIGH ) 
    return 0;
  while ( IR_read() == HIGH ) { 
    if ( micros()>>8 > lastRising + 600 ) return 0; 
  }
  lastFalling = micros()>>8;
  dhigh = lastFalling - lastRising;
  if ( dhigh < 11 )
    return 0;
  dhigh = 12;
  do {
    while ( IR_read() == LOW )  { 
      if ( micros()>>8 > lastFalling + 16 ) return 0; 
    }
    lastRising = micros()>>8;
    dlow = lastRising - lastFalling;
    while ( IR_read() == HIGH )  { 
      if ( micros()>>8 > lastRising + 16 ) {
        if ( bits == 7 ) break;
        return 0;
      }
    }
    lastFalling = micros()>>8;
    dhigh = lastFalling - lastRising;
    value <<= 1;
    if ( (dlow + 2)>>2 == 3 ) {
      //      Serial.print('1');
      value |= 1;
    }
    bits++;
  } 
  while (bits < 8);

  //  Serial.println();
  return value;
}








