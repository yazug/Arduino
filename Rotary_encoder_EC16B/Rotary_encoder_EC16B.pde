// In terminals-front top view, pins A, B from left
// are the signal output, and 
// c, the right one is the common ground.
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <SerLCD.h>

SerLCD lcd(2);

const int rotpin1 = 15;
const int rotpin2 = 16;

long lastmilli = 0;
byte state;
byte value;

void setup() {
  lcd.begin(16,2);
  lcd.backlightOn(66);
  //  Serial.begin(38400);

  pinMode(rotpin1, INPUT);
  digitalWrite(rotpin1, HIGH);
  pinMode(rotpin2, INPUT);
  digitalWrite(rotpin2, HIGH);

  state = digitalRead(rotpin1) | (digitalRead(rotpin2)<<1);
  value = 0;

  lcd.setCursor(0,0);
  lcd.print( state>>1&1, BIN);
  lcd.print(' ');
  lcd.print( state&1, BIN);
  lcd.setCursor(5,0);
  lcd.print(value, HEX);
}

void loop() {
  long deb;
  byte temp;

  temp = digitalRead(rotpin1) | (digitalRead(rotpin2)<<1);
  if ( temp != state ) {
    if ( state == 0b11 ) {
      if ( temp == 0b10 || temp == 0b01 ) {
        state = temp;
      }
    } 
    else if ( state == 0b10 ) {
      if ( (temp & 0b10) == 0 ) {
        state = 0b00;
        value ++;
      }
    } 
    else if ( state == 0b01 ) {
      if ( (temp & 0b01) == 0 ) {
        state = 0b00;
        value --;
      }
    } 
    else if ( state == 0b00 ) {
      if ( temp == 0b11 ) {
        state = 0b11;
      }
    }
    //
    lcd.setCursor(0,0);
    lcd.print( state>>1&1, BIN);
    lcd.print(' ');
    lcd.print( state&1, BIN);
    lcd.setCursor(5,0);
    lcd.print(value>>4&0x0f, HEX);
    lcd.print(value&0x0f, HEX);
  }
}






