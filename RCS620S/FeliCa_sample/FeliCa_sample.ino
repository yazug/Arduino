#include "RCS620S.h"
#include "ISO14443.h"

#define COMMAND_TIMEOUT 400
#define POLLING_INTERVAL 1000
#define LED_PIN 13

//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
RCS620S rcs620s(Serial1);

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
  ret = rcs620s.listPassiveTarget(tmp, 1,FELICA_SYSCODE_FCF);

  if (!ret) {
    Serial.println("polling...");
  } 
  else {
    Serial.print("IDm: ");
    for(i = 0; i < 8; i++) {
      Serial.print(rcs620s.idm[i]>>4, HEX);
      Serial.print(rcs620s.idm[i]&0x0f, HEX);
      Serial.print(' ');
    }
    Serial.println();
    //
    if ( (ret = rcs620s.requestService(FELICA_SERVICE_FCF)) ) {
      Serial.print("ret is ");
      Serial.println(ret);
      byte tmp[16];
      for(int b = 0; b < 4; b++) {
        ret = rcs620s.readWithoutEncryption(FELICA_SERVICE_FCF, b, tmp);
        Serial.print(b>>12&0x0f, HEX);
        Serial.print(b>>8&0x0f, HEX);
        Serial.print(b>>4&0x0f, HEX);
        Serial.print(b&0x0f, HEX);
        Serial.print(": ");
        for(int i = 0; i < 16; i++) {
          Serial.print(tmp[i]>>4, HEX);
          Serial.print(tmp[i]&0x0f, HEX);
          Serial.print(' ');
        }
        Serial.print("  ");
        for(int i = 0; i < 16; i++) {
          if ( isprint(tmp[i]) ) {
            Serial.print((char)tmp[i]);
          } 
          else {
            Serial.print('.');
          }  
        }
        Serial.println();
      }
    } 
    else {
      Serial.print("ret is ");
      Serial.println(ret);
    }
  }

  rcs620s.rfOff();
  digitalWrite(LED_PIN, LOW);
  delay(POLLING_INTERVAL);
}




