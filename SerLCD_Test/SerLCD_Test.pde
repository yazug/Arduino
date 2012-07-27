#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include "SerLCD.h"

SerLCD lcd(2, 16, 2);
void setup() {
  lcd.begin(9600);
  lcd.setCursor(4,0);
  lcd.println("Hi, there.");
  for (int i = 0; i < 3; i++) {
    delay(500);
    lcd.noDisplay();
    delay(500);
    lcd.display();
  }
}

void loop() {
  lcd.setCursor(0,1);
  lcd.println(millis()/(float)10000, 2);
  
  delayMicroseconds(50);
}
