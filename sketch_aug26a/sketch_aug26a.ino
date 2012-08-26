#include "Monitor.h"

Monitor mon(Serial);
char buf[128];
int len;

long blinktimer;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, OUTPUT);
  blinktimer = millis();
  Serial.begin(9600);
  mon << "Hi." << endl;
  
  buf[0] = 0;
  len = 0;
}

void loop() {
  if ( blinktimer + 500 < millis() ) {
    digitalWrite(13, !digitalRead(13) );
    blinktimer = millis();
  }
    
  if ( mon.readLine(buf, len, 127) ) {
    mon << "Got " << len << " line: " << buf << endl;
    mon.printHex(buf, 64, ' ');
    mon << endl;
    //
    len = 0;
    buf[0] = 0;
  }
}

