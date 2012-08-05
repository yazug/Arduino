#include "RCS620S.h"
#include "ISO14443.h"

#define COMMAND_TIMEOUT 400
#define POLLING_INTERVAL 1000
#define LED_PIN 13

//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
RCS620S rcs620s(Serial1);
byte cardtype = TypeA;

void setup() { 
  int ret;

  pinMode(LED_PIN, OUTPUT);   // for Polling Status
  digitalWrite(LED_PIN, LOW); 

  while( !Serial) ;
  Serial.begin(115200);      // Monitor

  // initialize RC-S620/S
  while(!rcs620s.init());  // blocking
}

void loop() {
  int ret, i;
  byte tmp[64];

  // Polling
  digitalWrite(LED_PIN, HIGH);
  rcs620s.timeout = COMMAND_TIMEOUT;
  cardtype = (cardtype == TypeF ? TypeA : TypeF);
  if ( !(ret = rcs620s.listPassiveTarget(tmp, cardtype,FELICA_SYSCODE_FCF)) ) {
    Serial.println("polling...");
  } 
  else {
    if ( cardtype == TypeF ) {
      Serial.print("FeliCa FCF IDm: ");
      for(i = 0; i < 8; i++) {
        Serial.print(rcs620s.idm[i]>>4, HEX);
        Serial.print(rcs620s.idm[i]&0x0f, HEX);
        Serial.print(' ');
      }
      Serial.println();
    } 
    else if ( cardtype == TypeA ) {
      Serial.print("Mifare UID: ");
      for(i = 0; i < rcs620s.idm[0]; i++) {
        Serial.print(rcs620s.idm[i+1]>>4, HEX);
        Serial.print(rcs620s.idm[i+1]&0x0f, HEX);
        Serial.print(' ');
      }
      Serial.println();
    }
    //
    if ( cardtype == TypeF && (ret = rcs620s.requestService(FELICA_SERVICE_FCF)) ) {
      read_FCF(tmp);
    } 
    else if ( cardtype == TypeA ) {
      Serial.println();
      Serial.println("Authenticate...");
      memcpy(tmp, "\x60\x04\xff\xff\xff\xff\xff\xff", 8);
      memcpy(tmp+8, rcs620s.idm+1, rcs620s.idm[0]);
      int ret = rcs620s.CommunicateThruEx(tmp, 8 + rcs620s.idm[0]); //+rcs620s.idm[0]);
      Serial.println();
      
    }
  }

  rcs620s.rfOff();
  digitalWrite(LED_PIN, LOW);
  delay(POLLING_INTERVAL);
}

void read_FCF(byte tmp[64]) {
  byte * p = tmp;
  for(int b = 0; b < 4; b++) {
    if( rcs620s.readWithoutEncryption(FELICA_SERVICE_FCF, b, p) ) {
      Serial.print(b>>12&0x0f, HEX);
      Serial.print(b>>8&0x0f, HEX);
      Serial.print(b>>4&0x0f, HEX);
      Serial.print(b&0x0f, HEX);
      Serial.print(": ");
      for(int i = 0; i < 16; i++) {
        Serial.print(p[i]>>4, HEX);
        Serial.print(p[i]&0x0f, HEX);
        Serial.print(' ');
      }
      Serial.print("  ");
      for(int i = 0; i < 16; i++) {
        if ( isprint(p[i]) ) {
          Serial.print((char)p[i]);
        } 
        else {
          Serial.print('.');
        }  
      }
      Serial.println();
    }
    p += 16;
  }
}



