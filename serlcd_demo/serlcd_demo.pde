/* 
	NOTE: you must: #include <SoftwareSerial.h>
	BEFORE including the SparkFunSerLCD.h header
*/

#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <SerLCD.h>

SerLCD lcd(2,2,16); // desired pin, rows, cols

void setup () {

  lcd.begin(9600);
  delay(200);
  lcd.clear();
  lcd.setCursor(0,4);
  lcd.print("Milliseconds");
  delay(1000);
  lcd.backlightOff();
  delay(1000);
  lcd.backlightOn();
  lcd.setCursor(0,7);
  lcd.print("m:");

  for (int b=0; b<101; b+=5) {
    lcd.brightness(b); b+=5;
    lcd.setCursor(1,9);
    lcd.print(millis());
    delay(500);
  }
/*
  led.cursorBlock();
  for (int x=5; x<9; x++) {
    led.pos(1,x);
    delay(500);
  }

  led.cursorUnderline();
  for (int x=10; x<15; x++) {
    led.pos(1,x);
    delay(500);
  }
*/
  lcd.noCursor();
}

void loop () {
  lcd.setCursor(1,9);
  lcd.print(millis());
}

