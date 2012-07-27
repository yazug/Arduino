#include <SPI.h>
#include "IOExpander.h"
#include <LiquidCrystal.h>
#include "HC595iox.h"
#include "HC595LCD.h"
#include "DS3234.h"

// initialize the library with the numbers of the interface pins
HC595LCD lcd(9, 7,6,5, 3,2,1,0);
DS3234 rtc(10);
long clockval;

const int led_red = 3; //PD3;
const int lcdlight = 5; //PD5;
const int led_blue = 6; //PD6;
const int led_green = 7; //PD7;
const int buzz = 8; //PB0;
const int button1 = 14; //PC0;
const int button2 = 15; //PC1;
const int button3 = 16; //PC2;

class PushButton {
  const int pinNo;
  boolean pulledUp;
  boolean pinStatus;
  long lastChanged, prevLastChanged;

public:
  PushButton(int pin) : 
  pinNo(pin) { 
    pulledUp = true;
  }

  PushButton(int pin, boolean pullup) : 
  pinNo(pin) { 
    pulledUp = pullup;
  }

  void init() {
    pinMode(pinNo, INPUT);
    digitalWrite(pinNo, (pulledUp? HIGH : LOW));
    pinStatus = digitalRead(pinNo);
    lastChanged = millis();
    prevLastChanged = lastChanged;
  }

  boolean isPushed() {
    if ( pulledUp ) return !pinStatus;
    return pinStatus;
  }

  boolean check() {
    boolean tmp = digitalRead(pinNo);
    if ( pinStatus != tmp && millis() > lastChanged + 12 ) {
      prevLastChanged = lastChanged;
      lastChanged = millis();
      pinStatus = tmp;
      return true;
    }
    return false;
  }

  boolean changedRecently() {
    if ( millis() < 100 + lastChanged ) {
      return true;
    }
    return false;
  }

  long holded() {
    return lastChanged - prevLastChanged;
  }

  Stream & printOn(Stream & out) {
    out.print( (isPushed()? "ON " : "OFF " ) );
    out.print( " after " );
    out.print(holded());
    out.print( " milli. " );
  }
};

PushButton buttons[] = {
  PushButton(button1),
  PushButton(button2),
  PushButton(button3)
  };
  const int buttons_length = 3;

void setup() {
  Serial.begin(9600);
  Serial.println("yes.");
  //
  SPI.begin();
  rtc.begin();
  rtc.getTime(clockval);
  lcd.begin(16, 2);
  lcd.print("hello, world!");
  //
  for(int i = 0; i < buttons_length; i++) {
    buttons[i].init();
  }
  delay(250);
  lcd.clear();
}

void loop() {
  boolean changed = false;
  int bttn = 0;
  for(int i = 0; i < buttons_length; i++) {
    buttons[i].check();
  }
  if ( buttons[0].changedRecently() ) {
    //buttons[0].printOn(Serial);
    //Serial.println();
    changed = true;
    bttn |= 1<<0;
  }
  if ( buttons[1].changedRecently() ) {
//    buttons[1].printOn(Serial);
//    Serial.println();
      changed = true;
      bttn |= 1<<1;
  }
  if ( buttons[2].changedRecently() ) {
//    buttons[2].printOn(Serial);
//    Serial.println();
    changed = true;
    bttn |= 1<<2;
  }
  if ( changed) {
    lcd.setCursor(0,0);
    lcd.print(bttn&1);
    lcd.print(bttn>>1&1);
    lcd.print(bttn>>2&1);
  }
  long tmp;
  rtc.getTime(tmp);
  if ( tmp != clockval ) {
    clockval = tmp;
    lcd.setCursor(0,1);
    lcd.print((long) clockval, HEX);
  }
}



