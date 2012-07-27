#include <OneWire.h>

/* DS18S20 Temperature chip i/o
 
 */

OneWire  ds(7);  // on pin 10
byte addr[8];

void setup(void) {
  Serial.begin(9600);
  ds.reset_search(addr);
}


void loop(void) {
  byte i;
  byte present = 0;
  byte data[12];
    
  if ( ds.searchExhausted() ) {
    Serial.println("Address exhausted; Reset the address.");
    ds.reset_search(addr);
    return;
  }
  ds.search(addr);
  
  Serial.print("Rom code ");
  for( i = 0; i < 8; i++) {
    Serial.print(addr[i] /16, HEX);
    Serial.print(addr[i] %16, HEX);
    if (i != 7)
      Serial.print(" ");
  }
  Serial.print(": "); 
  Serial.print(OneWire::crc8( addr, 8), HEX); 
  Serial.print(", ");
  
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
    default:
      Serial.println("Device is unknown/not a temp sensor; ");
      return;
  }
  Serial.println();

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end
  
  delay(1500);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad
//  Serial.print("P=");
//  Serial.print(present,HEX);
//  Serial.print(", ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
//    Serial.print(data[i], HEX);
//    Serial.print(" ");
  }
 
  Serial.print(0.0625*(int)((word)data[1]<<8 | data[0]), 2);
  Serial.print(" C");

  Serial.print(", CRC: ");
  Serial.print( OneWire::crc8( data, 9), HEX);
  Serial.println();

  delay(1500);
}

