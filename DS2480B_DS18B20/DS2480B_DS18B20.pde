#include <SoftwareSerial.h>
#include <OneWireMaster.h>

OneWireMaster bus(2,3); // rx. tx
//byte addr[8] = {0x28,0xe6,0xc2,0x21,0x2,0,0,0x94};
//byte addr[8] = {0x29,0x7e,0x3d,0x06,0,0,0,0xc9};
byte addr[8] = {
  0,0,0,0,0,0,0,0};

byte data[9];
void setup() {
  int i, r;
  Serial.begin(19200);

  Serial.println("Rom ");
  do {
    r = bus.search(addr);
    for (i = 0; i < 8; i++) {
      if (i>0)    Serial.print(".");
      if ( addr[i]/16 == 0 )
        Serial.print(' ');
      Serial.print(addr[i], HEX);
    }
    Serial.print("  ");
    Serial.print(OneWireMaster::crc8(addr,8), HEX);
    Serial.println("H");
    switch( addr[0] ) {
    case 0x10:
      Serial.print("DS18S20 family; ");
      break;
    case 0x22:
      Serial.print("DS1822 family; ");
      break;
    case 0x28:
      Serial.print("DS18B20 family; ");
      break;
    case 0x29:
      Serial.print("DS2408 family; ");
      break;
    default:
      Serial.print("Device is unknown; ");
      return;
    }
    Serial.println();
    
    bus.reset();
    bus.dataMode();
    bus.select(addr);
    bus.write(0x44);
    delay(1500);
    bus.reset();
    bus.dataMode();
    bus.select(addr);
    bus.write(0xBE);
    delayMicroseconds(2500);
    for ( i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = bus.read();
    }
//    Serial.print(data[i], HEX);
//    Serial.print(" ");
  //s}

  } 
  while ( !r );


}

void loop() {
}



