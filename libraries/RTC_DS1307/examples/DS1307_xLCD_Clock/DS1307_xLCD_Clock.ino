/*
  */

// include the library code:
#define TWI_FREQ 400000L
#include <Wire.h>
#include "IOExpander.h"
#include "MCP23009.h"
#include "xLCD.h"
#include "DS1307.h"

// initialize the library with the numbers of the interface pins
xLCD lcd(0x00, 6, 5, 4, 3, 2, 1, 0, 7);
DS1307 rtc = DS1307();
long time;

void setup() {
  //  Serial.begin(57600);
  Wire.begin();
  // set up the LCD's number of rows and columns: 

  //  Serial.println("B");
  lcd.begin(16, 2);
  rtc.init();

  //  Serial.println("F");
  //  perpetual:    goto perpetual;

  lcd.backlightOn();

  // Print a message to the LCD.
  lcd.home();
  lcd.print("hello, world!");
  delay(3000);
  for (int i = 0; i < 3; i++) {
    lcd.noDisplay();
    delay(500);
    lcd.display();
    delay(500);
  }
  lcd.clear();
  rtc.getTime(time);
  //perpetual:    goto perpetual;
}

void loop() {
  long tmp;
  //lcd.command(c);
  //  delay(5);
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);
  rtc.getTime(tmp);
  if ( tmp != time ) {
    time = tmp;
    lcd.print(time, HEX);
  }
}


