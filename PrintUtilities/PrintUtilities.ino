#include <PrintUtilities.h>

using namespace prt;

void setup() {
  Serial.begin(19200);
  Serial.println("hi.");
}

void loop() {
  Serial.println(printOn(random(10000) - 5000, "+00 00 0"));
  delay(500);  
}

