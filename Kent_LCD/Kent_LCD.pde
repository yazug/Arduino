/*
 * Kent LCD Example code
 * Copyright (c) 2010 SparkFun Electronics.  All right reserved.
 * Written by Chris Taylor
 *
 * This code was written to demonstrate Large (320x240) Kent LCD Display 
 * 
 * This code will print vertical bars on the Kent LCD
 * Note: For this code to work properly with the Small Kent display, change
 * the loop max value in line 55. All other procedures work for both displays.
 *
 * http://www.sparkfun.com
 */
#include <util/delay.h>

#include "SPI.h"
#include "Kent_LCD.h"
#include "Arial14.h"

KentChLCD lcd(10);

#define LCD 10 //cs pin

void setup() {
  Serial.begin(9600); // For debugging

  lcd.begin();
  Serial.println("Kent LCD Test");
  lcd.reset();
  Serial.println("Kent LCD reset");

  lcd.clear();
  _delay_ms(100);

  // Send data to onboard RAM
  word c, r;
  byte bmap[] = {
    0x01,0x71,0x09,0x05,0x03,0x00, 0x00, 0x00  };
  byte pixs[8];

  for (int y = 0; y < 8; y++) {
    pixs[y]= 0;
    for(int x= 0; x < 8; x++){
      pixs[y] |= (bmap[x]&(1<<y)?0x80>>x:0);
    }
  }

  r = 31;
  c = 55;
  for(int i = 0; i < sizeof(pixs); i++) {
    lcd.write(r+i, c, &pixs[i], 1);
  }

  Serial.println("Data transfer complete.");

  lcd.update();

  Serial.println("Screen update complete.");

}

void loop() {
}

