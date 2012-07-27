#include "TimerCounter.h"

const long COUNTPERSEC = 2;

const byte sw0pin = 13;
const byte sw1pin = 12;
byte swstat = LOW;

void setup() {
  TimerCounter1::mode(0);
  TimerCounter1::prescaler(4);

  pinMode(sw1pin, OUTPUT);
  pinMode(sw0pin, OUTPUT);
  digitalWrite(sw1pin, swstat);
  digitalWrite(sw0pin, !swstat);
  
//  Serial.begin(9600);
}

unsigned long count;
void loop() {
  unsigned int an0 = analogRead(2);
  unsigned int an1 = analogRead(3);
  unsigned int t = TimerCounter1::counter();
  if (!(t < 15625)) {
    t -= 15625;
    TimerCounter1::counter(t);
    count++;
//    Serial.print("an0: ");
  //  Serial.print(an0);
 //   Serial.print(", an1: ");
 //   Serial.print(an1);
 //   Serial.println();
  } else {
    return;
  }

  if (swstat == LOW) {
    if (count > (an0>>2)) {
      swstat = HIGH;
      digitalWrite(sw1pin, swstat);
      digitalWrite(sw0pin, !swstat);
      count = 0;
    }
  } 
  else {
    if (count > (an1>>2)) {
      swstat = LOW;
      digitalWrite(sw1pin, swstat);
      digitalWrite(sw0pin, !swstat);
      count = 0;
    }
  }
}


