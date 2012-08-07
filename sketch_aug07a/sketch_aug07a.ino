#include "Monitor.h"

char tmp[32];
Monitor mon(Serial, tmp);

void setup() {
  Serial.begin(9600);

  mon << "hi. " << endl;
  mon.radix((word)112) << 'h' << endl;
  mon.radix((unsigned long)112) << 'h' << endl;

}

void loop() {
}

