#include "AES128.h"

byte key[] = { 
  0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 
  0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf 
};
AES128 aes(key);

void setup() {
  Serial.begin(9600);
  Serial.flush();
  while ( Serial.available() ) Serial.read();
}

void loop() {
  byte buf[32];
  long msec;
  
  if (SerialReadText(buf, 16) ) {
    showBytes(buf, 16);
    Serial.println();
    msec = millis();
    aes.encrypt(buf);
    msec = millis() - msec;
    showBytes(buf, 16);
    Serial.println();
    Serial.print(msec);
    Serial.println(" milli seconds.");
    msec = millis();
    aes.decrypt(buf);
    msec = millis() - msec;
    showBytes(buf, 16);
    Serial.println();
    Serial.print(msec);
    Serial.println(" milli seconds.");
  }
}

int SerialReadText(byte * buf, int num) {
  const int WAIT_LIMIT = 50;
  int count = 0;
  byte c;
  long wait = millis();
  if (!Serial.available())
    return 0;
  for ( ; count < num; ) {
    if ( Serial.available() ) {
      c = Serial.read();
    } else {
      c = 0;
    }      
    *buf++ = c;
    count++;
    while ( ! Serial.available() ) {
      if ( wait + WAIT_LIMIT < millis() || wait > millis() ) 
        break;
    }
  }
  return count;
}

void showBytes(byte * b, int n) {
  int i;
  for ( i = 0; i < n ; i++) {
    if ( isprint(*b) ) {
      Serial.print((char)*b++);
      Serial.print("  ");
    } else {
      Serial.print(*b>>4, HEX);
      Serial.print(*b&0x0f, HEX);
      b++;
      Serial.print(' ');
    }
  }
}

