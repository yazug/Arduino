#include "SPI.h"
#include "DataFlash_SPI.h"

DataFlash dflash(5); 
char text[] = "  If thou beest he; \nBut O how fall'n! how chang'd From him, \nwho in the happy Realms of Light \nCloth'd with transcendent brightnes didst outshine \nMyriads though bright: \nIf he whom mutual league, \nUnited thoughts and counsels, equal hope, \nAnd hazard in the Glorious Enterprize, \nJoynd with me once, now misery hath joynd \nIn equal ruin: into what \nPit thou seest \nFrom what highth fal'n, so much the stronger provd \nHe with his Thunder: and till then who knew \nThe force of those dire \nArms? yet not for those \nNor what the Potent Victor in his rage \nCan else inflict do I repent or change, ";
unsigned long addr = 0;

void setup() {
    byte not_used_cs[] = { 
    4, 9, 10   };
  for (int i = 0; i < sizeof(not_used_cs); i++) {
    pinMode(not_used_cs[i], OUTPUT);
    digitalWrite(not_used_cs[i], HIGH);
  }

  Serial.begin(57600);

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
  goto skipped;
  Serial.print("Writing text... ");
  Serial.println(text);
  dflash.write((const char*) text);
  dflash.write('\n');
  Serial.println("finished. ");
//  dflash.flush();
  dflash.reset();
skipped:
  delay(1000);

}

void loop() {
  unsigned int i;
  byte c;
  c = dflash.read();
  if ( addr % 64 == 0 ) {
    Serial.println();
    Serial.print(addr/528, HEX);
    Serial.print(".");
    Serial.print(addr%528, HEX);
    Serial.println(": ");
  }
//  Serial.print(" ");
  if ( 31 < c && c < 127) {
    Serial.print((char) c);
  } else {
    Serial.print("\\");
    Serial.print(c, HEX);
  }
  addr++;
  delay(20);
}


