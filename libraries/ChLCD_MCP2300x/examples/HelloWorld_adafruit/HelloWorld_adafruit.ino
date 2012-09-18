/*
  LiquidCrystal Library - Hello World
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the 
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 
 This sketch prints "Hello World!" to the LCD
 and shows the time.
 
 The circuit:
 * LCD RS pin to digital pin 4
 * LCD Enable pin to digital pin 5
 * LCD D4 pin to digital pin 0
 * LCD D5 pin to digital pin 1
 * LCD D6 pin to digital pin 2
 * LCD D7 pin to digital pin 3
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe 
 
 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#define TWI_FREQ 400000L
#include <Wire.h>
#include "IOExpander.h"
#include "MCP23009.h"
#include "CharacterLCD.h"
#include "xLCD.h"

// initialize the library with the numbers of the interface pins
//xLCD lcd(0b100111, 4, 5, 2, 3, 0, 1);
//xLCD lcd(0x00, 6, 5, 4, 3, 2, 1, 0, 7);
xLCD lcd(0x00, 1, 0xff, 2, 3, 4, 5, 6, 7); // for adafruit i2c/spi lcd adapter
void setup() {
  //  Serial.begin(57600);
  Wire.begin();
  // set up the LCD's number of rows and columns: 

  //  Serial.println("B");
  lcd.begin(20, 4);

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
  //perpetual:    goto perpetual;
}

long t = 0;
void loop() {
  //lcd.command(c);
  //  delay(5);
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);
  // print the number of seconds since reset:
  lcd.print(float(millis())/1000,3);
  
  if ( millis() - t >= 1000 ) {
    t = millis();
    lcd.setCursor(0, 1);
    lcd.print( ((long)analogRead(0)*5000/1024 - 500)/(float)10 );
  }
}


