#include <avr/pgmspace.h>

#include "SPI.h"
#include "DataFlash_SPI.h"

#include "text.h"

DataFlash dflash(5); 
unsigned long addr = 0;
char buffer[16];

void setup() {
  byte not_used_cs[] = { 
    4, 9, 10 
  };
  for (int i = 0; i < sizeof(not_used_cs); i++) {
    pinMode(not_used_cs[i], OUTPUT);
    digitalWrite(not_used_cs[i], HIGH);
  }

  Serial.begin(19200);
  Serial.flush();
  Serial.println("Hi.");

  SPI.begin();
  if ( ! dflash.begin() ) {
    Serial.println(dflash.status(), BIN);
    Serial.println("Perhaps initialization has been failed.");
    while (true);
  }
  Serial.print("Page Size in bytes: ");
  Serial.println(dflash.pageSize());
  Serial.print("Address Bits per page: ");
  Serial.println(dflash.pageBits());
  Serial.println();
  //goto skipped;
  Serial.println("Writing text... ");
  word textlength = strlen_P(text);
  for(int i = 0; i < textlength; i += 16) {
    memcpy_P(buffer, text+i, 16);
    dflash.write(i, (byte*) buffer, 16);
   // dflash.read(i, (byte*) buffer, 16);
    buffer[16] = 0;
    Serial.print((char*)buffer);
  }
  //  dflash.write('\n');
  Serial.println(" writing finished. ");
  dflash.flush();
skipped:
  delay(1000);

  char c;
  for( addr = 0; addr < strlen(text); addr++) {
    if ( (addr % 0x100) == 0 ) {
      Serial.println();
      Serial.print(addr>>16&0x0f, HEX);
      Serial.print(addr>>12&0x0f, HEX);
      Serial.print(addr>>8&0x0f, HEX);
      Serial.print(addr>>4&0x0f, HEX);
      Serial.print(addr& 0x0f, HEX);
      Serial.println(": ");
    }
    c = dflash.read(addr);
    if ( c == 0 ) {
      Serial.println();
      continue;
    }
    Serial.print(c);
    delay(5);
  }
  Serial.println(" --- end ----");
}

void loop() {
}






