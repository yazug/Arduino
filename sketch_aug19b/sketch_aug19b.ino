#include <Monitor.h>
Monitor moni(Serial);

void setup() {
  Serial.begin(19200);
  
  moni.print( *((long*)"TSET"),HEX);
  moni << endl;
  moni.print( *((long*)"CSET"),HEX);
  moni << endl;
  moni.print( *((long*)"LIST"),HEX);
  moni << endl;
  moni.print( *((long*)"QUIT"),HEX);
  moni << endl;
}

void loop() {}
