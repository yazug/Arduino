#include <Wire.h>
#include "PN532_I2C.h"

#include <SPI.h>


#define IRQ   (2)
#define RESET (0xff)  // Not connected by default on the NFC Shield
PN532 nfc(PN532::I2C_ADDRESS, IRQ, RESET);

const byte cardkey_b[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
const byte key_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };

long prev;
NFCCard card;

void setup() {
  Serial.begin(19200);
  Serial.println("yes.");

  Wire.begin();
  nfc.begin();

  unsigned long r = nfc.getFirmwareVersion();
  Serial.println();
  Serial.println(r, HEX);
  if (! r ) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  } 

  // Got ok data, print it out!
  Serial.print("Found chip PN5"); 
  Serial.println(r & 0xff, HEX);
  Serial.print("Firmware ver. "); 
  Serial.print(r>>8 & 0xFF, DEC); 
  Serial.print('.'); 
  Serial.println(r>>16 & 0xFF, DEC);

  Serial.println("SAMConfiguration");
  nfc.SAMConfiguration();

  delay(1000);
}

void loop() {
  byte c;
  byte buf[48];
  NFCCard tc;

  if ( millis() > prev + 100 ) {
    prev = millis();
    c = nfc.listPassiveTarget(buf, PN532::BaudrateType_106kbitTypeA);
    if ( c != 0 ) {      
      Serial.print(c, DEC);
      Serial.println(" cards detected. ");
      tc.setInListPassiveTarget(buf, 1, PN532::Type_Mifare);
      Serial.println();
      Serial.print("type = ");
      Serial.print(tc.type, HEX);
      Serial.print(", ID Len = ");
      Serial.println(tc.NFCIDLength, DEC);
      Serial.print("NFCID = ");
      for(int i = 0; i < tc.NFCIDLength; i++) {
        Serial.print(tc.data[i]>>4, HEX);
        Serial.print(tc.data[i]&0x0f, HEX);
        Serial.print(" ");
      }
      Serial.println();
      if ( !(card == tc) ) {
        card.set(tc);
        /*
        Serial.println(" Mifare");
         Serial.print("Card UID bytes: ");
         for(int i = 0; i < card.NFCIDLength; i++) {
         Serial.print(card.data[i]>>4&0x0f,HEX);
         Serial.print(card.data[i]&0x0f,HEX);
         }
         Serial.println();
         */
        if ( nfc.mifare_AuthenticateBlock(card.NFCID1, card.NFCIDLength, 4, cardkey_b) ) {
          Serial.println("Authenticated.");
          if ( nfc.mifare_ReadDataBlock(4,buf) ) {
            Serial.println("Block contents: ");
            nfc.printHexString(buf, 16);
            Serial.print(' ');
            for(int i = 0; i < 16; i++) {
              if ( isprint(buf[i]) ) {
                Serial.print((char) buf[i]);
              } else {
                Serial.print(' ');
              }
            }
            Serial.println();
          }
        } else {
          Serial.print("Auth failed. ");
          Serial.println(nfc.lastStatus, HEX);
          Serial.flush();
        }
        Serial.println();
      }
    } 
    else {
      card.clear();
    }
  }
  delay(1000);
}




















