#include "Monitor.h"

Monitor mon(Serial);

void setup() {
  Serial.begin(9600);

  mon << "hi. " << endl;
  mon << 23 << endl;
  mon.print((word)112);
  mon << endl;
  long val = 112;
  mon.printBytes((byte*)&val, 4);
  mon << endl;
  mon.print(val, HEX);
  mon << endl;
  mon.printBytes((byte*) "Iizuka ?", 9);
 mon << endl;
}

void loop() {
}


