#include <stdio.h>
#include "Monitor.h"

const int row_pin[] = {
  9, 3, 4, 6, 0};
const int col_pin[] = {
  7, 8, 5, 0};
const char keypad[] = { 
  '1','2','3', '4','5','6', '7','8','9', '*','0','#',0 };
  
int scanKeypad() {
  boolean buttonUp = false;
  for (int r = 0; r < 4; r++) {
    for(int c= 0; c < 3; c++) {
      pinMode(col_pin[c],OUTPUT);
      digitalWrite(col_pin[c],LOW);
      delay(5);
      if ( !digitalRead(row_pin[r]) ) {
        buttonUp = true;
        while ( !digitalRead(row_pin[r]) );
      }
      pinMode(col_pin[c],INPUT);
      digitalWrite(col_pin[c],HIGH);
      if ( buttonUp )
        return keypad[r*3+c];
    }
  }
  return keypad[12];
}

long value;
char message[10];

Monitor mon(Serial);

void setup() {
  Serial.begin(9600);
  for (int i=0; i < 4; i++) {
    pinMode(row_pin[i], INPUT);
    digitalWrite(row_pin[i], HIGH);
  }
  for (int i=0; i < 3; i++) {
    pinMode(row_pin[i], INPUT);
    digitalWrite(row_pin[i], HIGH);
  }
  value = 0;
  message[0] = 0;
  mon << mon.cr;
}

void loop() {
  int b;
  long updated = value;

  b = scanKeypad();
  if ( b ) {
    if ( b >= '0' && b <= '9' ) {
      tone(2,3136,10);
      if ( value < 10000000 )
        updated = value*10+ (b - '0');
    } 
    else if ( b == '*' ) {
      if ( value > 0 ) 
        tone(2,2794,10);
      else 
        tone(2,1600,100);
      updated = value/10;
    } 
    else if ( b == '#' ) {
      tone(2,3136,20);
      delay(30);
      tone(2,3136,20);
      updated = 0;
    }
    if ( updated != value) {
      value = updated;
      sprintf(message,"% 8ld", value/10);
      mon << (message+1); // skip the first blank reserved for the sign (-)
      mon << '.';
      sprintf(message,"%d", value%10);
      mon << message;
      mon << '\r';
    }
  }
}

