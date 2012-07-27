/*
 * Copyright 2010 Sony Corporation
 *
 * This sample program simply lit the LED that is connected to the pin 13 when
 * the FeliCa Plug detects RF from external reader/writers.
 */

#include <EEPROM.h>
#include <FeliCaPlug.h>
#include <inttypes.h>

FeliCaPlug plug;

void setup()
{
  uint8_t dfc[2] = {0xff, 0xe0};
  uint8_t userParam[4] = {0x00, 0x11, 0x22, 0x33};
  
  plug.initPort();
  plug.setParam(FELICA_PLUG_MODE_FB, dfc, userParam);
  plug.setLEDPin(FELICA_PLUG_LED_PIN);
  plug.setFBCallback(fbCallback);
}

void loop()
{
  plug.doLoop();
}

void fbCallback(uint8_t* numOfBlocks,
                const uint8_t* blockList,
                uint8_t* blockData)
{
  for (int i = 0; i < (*numOfBlocks * 16); i++) {
    blockData[i] += 0x11;
  }
}

