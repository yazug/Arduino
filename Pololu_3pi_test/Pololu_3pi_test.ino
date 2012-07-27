#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 4, 9, 12, 13, 7);
const int pin_RW = 8;

long swatch;

void setup() {
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  
  lcd.begin(8,2);

  tone(10, 880, 300);
  delay(100);
  tone(10, 1760, 300);

  swatch = millis();
}

void loop() {
  if ( millis() > swatch + 500 ) {
    swatch = millis();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("F ");
    lcd.print(analogRead(6));
    lcd.setCursor(0,1);
    lcd.print("R ");
    lcd.print(analogRead(7));
  }
}


