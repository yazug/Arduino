#include <OneWire.h>

/* DS18S20 Temperature chip i/o
 
 */

OneWire  ds(7);  // on pin 10
byte addr[8];

void setup(void) {
  // initialize inputs/outputs
  // start serial port
  Serial.begin(9600);
}


void loop(void) {
  byte i;
  byte present = 0;
  byte data[12];
  
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
  
  if ( !ds.search(addr)) {
      //Serial.print(".");
      ds.reset_search();
      return;
  }
  
  Serial.print("Rom code ");
  for( i = 0; i < 8; i++) {
    Serial.print(addr[i] >> 8, HEX);
    Serial.print(addr[i] & 0x0f, HEX);
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
      Serial.print("Device is unknown; ");
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

  Serial.print("P=");
  Serial.print(present,HEX);
  Serial.print(", ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
//    Serial.print(data[i], HEX);
//    Serial.print(" ");
  }
  
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit) // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25

//  Whole = Tc_100 / 100;  // separate off the whole and fractional portions
//  Fract = Tc_100 % 100;
  Tc_100 += 5;

  if (SignBit) // If its negative
  {
     Serial.print("-");
  }
  Serial.print(Tc_100 / 100);
  Serial.print(".");
  Serial.print((Tc_100 % 100)/10);
  Serial.print(" C");

  Serial.print(" : ");
  Serial.print( OneWire::crc8( data, 9), HEX);
  /*
  Serial.print(".");
  if (Fract < 10)
  {
     Serial.print("0");
  }
  Serial.print(Fract);
*/
Serial.println();
  delay(1500);
}

