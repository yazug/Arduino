#include "Monitor.h"

char tmp[32];
Monitor mon(Serial, tmp);

void setup() {
  Serial.begin(9600);

  mon << "hi. " << endl;
  mon << 23 << endl;
  mon.print((word)112);
  mon << 'h' << endl;
  mon.print((unsigned long)112) << 'h' << endl;
  mon.print((byte*) "Iizuka ?", 9) << endl;
}

void loop() {
}

