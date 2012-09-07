#include "SPI.h"
#include "DataFlash_SPI.h"

DataFlash dflash(5); 
char text[] = "Paradise Lost: \n John Milton\n  If thou beest he; \nBut O how fall'n! how chang'd From him, \nwho in the happy Realms of Light \nCloth'd with transcendent brightnes didst outshine \nMyriads though bright: \nIf he whom mutual league, \nUnited thoughts and counsels, equal hope, \nAnd hazard in the Glorious Enterprize, \nJoynd with me once, now misery hath joynd \nIn equal ruin: into what \nPit thou seest \nFrom what highth fal'n, so much the stronger provd \nHe with his Thunder: and till then who knew \nThe force of those dire \nArms? yet not for those \nNor what the Potent Victor in his rage \nCan else inflict do I repent or change, ";
unsigned long addr = 0;

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
    Serial.println(text);
    dflash.write(0, (byte*)text, sizeof(text));
  //  dflash.write('\n');
  Serial.println("finished. ");
  dflash.flush();
skipped:
  delay(1000);

  char c;
  for( addr = 0; addr < sizeof(text); addr++) {
    if ( (addr % 0x100) == 0 ) {
      Serial.println();
      Serial.print(addr>>16&0x0f, HEX);
      Serial.print(addr>>12&0x0f, HEX);
      Serial.print(addr>>8&0x0f, HEX);
      Serial.print(addr>>4&0x0f, HEX);
      Serial.print(addr& 0x0f, HEX);
      Serial.println(": ");
      Serial.println();
    }
    c = dflash.read(addr);
    if ( c == 0 ) {
      Serial.println();
      continue;
    }
    Serial.print(c);
    delay(20);
  }
  Serial.println(" --- end ----");
}

void loop() {
}





