//#include <Pololu3pi.h>
//#include <PololuQTRSensors.h>
#include <OrangutanAnalog.h>


void setup() {
  pinMode(19, INPUT);
  digitalWrite(19, LOW);
  Serial.begin(9600);
  Serial.println("Hi.");
}



void loop() {
  Serial.println(analog_read(5));
  delay(500);
}

