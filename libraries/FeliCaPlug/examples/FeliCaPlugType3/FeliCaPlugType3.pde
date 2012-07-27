/*
 * Copyright 2010 Sony Corporation
 *
 * This sample lets the FeliCa Plug behave as the NFC Type3 Tag.
 *
 * Refer to the NFC Forum official page for more information about the NFC 
 * Type3 Tag.
 * 
 * - http://www.nfc-forum.org  
 */

#include <EEPROM.h>
#include <FeliCaPlug.h>
#include <inttypes.h>

FeliCaPlug plug;

uint8_t blockData[FELICA_PLUG_EEPROM_MAX_NUM_OF_BLOCKS * 16];

void setup()
{
  uint8_t dfc[2] = {0xff, 0xe0};
  uint8_t userParam[4] = {0x01, 0x23, 0x45, 0x67};
  
  plug.initPort();
  plug.setParam(FELICA_PLUG_MODE_TYPE3, dfc, userParam);
  plug.setLEDPin(FELICA_PLUG_LED_PIN);
  plug.setBlockData(blockData, FELICA_PLUG_EEPROM_MAX_NUM_OF_BLOCKS, 0, 1);
}

void loop()
{
  plug.doLoop();
}

