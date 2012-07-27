// include the library code:
//
#include <SPI.h>
#include "IOExpander.h"
#include <LiquidCrystal.h>
#include "HC595iox.h"
#include "HC595LCD.h"
#include "DS3234.h"

#include "Switch.h"

const int PIN_BUZZ = 8; // PB0
const int PIN_595_CS = 9; //PB1
const int PIN_RTC_CS = 10; //PB2
const int PIN_LCDLIGHT = 5; // PD5
const int PIN_LEDRED = 3; //PD3
const int PIN_LEDGREEN = 7; //PD7
const int PIN_SW1 = 14; //PC0;
const int PIN_SW2 = 15; //PC1;
const int PIN_SW3 = 16; //PC2;

HC595LCD lcd(PIN_595_CS, 7,6,5, 3,2,1,0);
DS3234 rtc(PIN_RTC_CS);
long time;
long date;

enum {
  SW1 = 0,
  SW2,
  SW3 };
const int numberOfButtons = 3;

  Switch buttons[] = { PIN_SW1, PIN_SW2, PIN_SW3 };

void setup() {

  for(int i = 0; i < numberOfButtons; i++) {
    buttons[i].init();
  }
  //
  SPI.begin();
  lcd.begin(16, 2);
  rtc.begin();
  //
  lcd.print("hello, world!");
  delay(250);
  //
  rtc.getTime(time);
  rtc.getCalendar(date);
  
  lcd.clear();
}

void loop() {
  buttons[SW1].check();
  if ( buttons[SW1].changed((long)66) || buttons[SW1].isLow() ) {
    lcd.setCursor(0,1);
    lcd.print(buttons[SW1].still());
    lcd.print(" ");
    lcd.print((int)buttons[SW1].state());
  }
}









