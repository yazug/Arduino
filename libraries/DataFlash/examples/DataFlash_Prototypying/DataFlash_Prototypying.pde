#include "SPI.h"
#include "DataFlash.h"

DataFlash dflash(10); 
char text[] = "  If thou beest he; But O how fall'n! how chang'd From him, who in the happy Realms of Light Cloth'd with transcendent brightnes didst outshine Myriads though bright: If he whom mutual league, United thoughts and counsels, equal hope, And hazard in the Glorious Enterprize, Joynd with me once, now misery hath joynd In equal ruin: into what Pit thou seest From what highth fal'n, so much the stronger provd He with his Thunder: and till then who knew The force of those dire Arms? yet not for those Nor what the Potent Victor in his rage Can else inflict do I repent or change, ";
unsigned long addr = 0;

void setup() {
  
  SPI.begin(3,0);
  Serial.begin(38400);
  if ( ! dflash.init() ) {
    Serial.println(dflash.status(), BIN);
    Serial.println("Perhaps initialization has been failed.");
    while (true);
  }
  Serial.print("Page Size in bytes: ");
  Serial.println(dflash.pageSize());
  Serial.print("Address Bits per page: ");
  Serial.println(dflash.pageBits());
  Serial.println();
  delay(1000);

}

void loop() {
  unsigned int i;
  byte c;
  //dflash.write( text[addr%sizeof(text)]);
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


