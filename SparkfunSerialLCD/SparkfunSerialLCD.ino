#include <SoftwareSerial.h>
#include "LCD.h"
#include "SerialLCD.h"

SoftwareSerial ser(3,3);
SerialLCD lcd(ser);

void setup() {
  Serial.begin(9600);
  pinMode(3, OUTPUT);
//  ser.begin(9600);
  lcd.begin(9600);
  delay(500);
  lcd.clear();
  lcd.println("Hello, this is Kelly.");
  delay(1000);
  lcd.println("I'm here.");
  delay(1000);
}

void loop() {
  lcd.print(millis());
  lcd.println("  ");
  delay(1000);
}

