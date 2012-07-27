/*
 * Copyright 2010 Sony Corporation
 *
 * This sample lets a LCD show a message that is written into the FeliCa Plug
 * from an external FeliCa Reader/Writer. The string must be written into the
 * FeliCa Plug in form of NDEF Type3 Tag.  
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

uint8_t blockData[16 * 12] = {
  0x10, 0x0c, 0x0c, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x35};

void setup()
{
  uint8_t dfc[2] = {
    0xff, 0xe0};
  uint8_t userParam[4] = {
    0x00, 0x11, 0x22, 0x33};

  plug.initPort();
  plug.setParam(FELICA_PLUG_MODE_TYPE3, dfc, userParam);
  plug.setLEDPin(FELICA_PLUG_LED_PIN);
  plug.setBlockData(blockData, 12, 0, 0);
  plug.setFTReadCallback(ftReadCallback);
  plug.setFTWriteCallback(ftWriteCallback);

  lcd.begin(2, 16) ;
  lcd.clear();
}

void loop()
{
  plug.doLoop();
}

void ftReadCallback(uint16_t blockNum) 
{

}

void ftWriteCallback(uint16_t blockNum) 
{ 
  // if the WriteF flag is OFF, writing type3 tag data has completed
  if (blockData[9] == 0x00) {

    lcd.clear();

    int i;
    for (i = 16; i < 32; i++) {
      if (blockData[i] == 0) {
        break; 
      }
      lcd.print(blockData[i], BYTE);
    }
    lcd.setCursor(0, 1);
    for (i = 32; i < 48; i++) {
      if (blockData[i] == 0) {
        break; 
      }
      lcd.print(blockData[i], BYTE);
    }
  }
}


