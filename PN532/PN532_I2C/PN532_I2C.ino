#include <Wire.h>
#include "PN532_I2C.h"

#define IRQ   (2)
#define RESET (3)  // Not connected by default on the NFC Shield

PN532 nfc(PN532::I2C_ADDRESS, IRQ);

void setup() {
  Wire.begin();
  nfc.begin();
  
  Serial.begin(19200);
  long verstr;
  verstr = nfc.getFirmwareVersion();
  Serial.print("Found chip PN5 "); 
  Serial.println(verstr & 0xFF, HEX); 
  Serial.print("Firmware ver. "); 
  Serial.print(verstr>>8 & 0xFF, HEX); 
  Serial.print('.'); 
  Serial.print(verstr>>16 & 0xFF, HEX);
  Serial.print('.'); 
  Serial.print(verstr>>24 & 0xFF, HEX);
  Serial.println();
}

void loop() {
}


