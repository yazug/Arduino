/*
 * Copyright 2010 Sony Corporation
 *
 * This sample lets a LCD show a message that is written into the FeliCa Plug
 * from an external FeliCa Reader/Writer. 
 *
 * This sample program requires Arduino Liquid Crystal Library (usually 
 * bundled with Arduino Software) to be built.  Also, a supported LCD must be
 * connected to Arduino.  Please, look at the code carefully how the pins on 
 * Arduino are connected to each pin on the LCD.
 *
 * Refer to the Arduino official page for more information about the LCD 
 * library. 
 *
 * - http://www.arduino.cc/en/Tutorial/LCDLibrary 
 */

#include <EEPROM.h>
#include <FeliCaPlug.h>
#include <inttypes.h>
#include <LiquidCrystal.h>

FeliCaPlug plug;
LiquidCrystal lcd(0,1,4,5,6,7,8); // (rs, rw, enable, d4, d5, d6, d7) 

void setup()
{
  uint8_t dfc[2] = {
    0xff, 0xe0  };
  uint8_t userParam[4] = {
    0x00, 0x11, 0x22, 0x33  };

  plug.initPort();
  plug.setParam(FELICA_PLUG_MODE_FB, dfc, userParam);
  plug.setLEDPin(FELICA_PLUG_LED_PIN);
  plug.setFBCallback(fbCallback);
 
  lcd.begin(2, 16) ;
  lcd.clear();
  lcd.print("Initialized");
}

void loop()
{
    plug.doLoop();
}

void fbCallback(uint8_t* numOfBlocks,
                const uint8_t* blockList,
                uint8_t* blockData)
{  
  lcd.clear();

  int i;
  for (i = 0; i < 16; i++) {
    if (blockData[i] == 0) {
      break; 
    }
    lcd.print(blockData[i], BYTE);
  }
  
  lcd.setCursor(0, 1);
  for (i = 16; i < 32; i++) {
    if (blockData[i] == 0) {
      break; 
    }
    lcd.print(blockData[i], BYTE);
  }
}

