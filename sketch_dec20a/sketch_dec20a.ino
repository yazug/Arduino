#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include "SerLCD.h"

SerLCD lcd(3);

void setup() {
  pinMode(4, INPUT);
  digitalWrite(4, HIGH);
  pinMode(5, INPUT);
  digitalWrite(5, HIGH);
pinMode(13, OUTPUT);

  pinMode(3, OUTPUT);
  lcd.begin(9600, 16, 2);
  lcd.clear();
  lcd.print("Hi");
}

void loop() {
  long stt, stp;
  long sm, sp;
      digitalWrite(13, digitalRead(4));    
  if ( digitalRead(4) == LOW ) {
    sm = micros();
    stt = millis();
    while ( digitalRead(5) == HIGH );
    sp = micros();
    stp = millis();
    lcd.clear();
    lcd.print(stp -stt);
    lcd.print(" mm");
    lcd.setCursor(0,1);
    lcd.print((sp-sm)/1000.0,3);
    lcd.print(" mu sec");
  }
}

