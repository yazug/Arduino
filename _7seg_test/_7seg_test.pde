#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include "SerLCD.h"

SerLCD slcd(2, 9600);

void setup() {
  slcd.begin(16, 2);
}

void loop() {
  char msg4[5];
  
  slcd.home();
  slcd.print(millis()%1000);
  slcd.println("   ");
  delay(1000);
}

