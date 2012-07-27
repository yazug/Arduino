#include <OneWire.h>
#include <SerialMonitor.h>

/*
  DS18S20 Temperature chip i/o
 */

OneWire  ds(7);  // on pin 10
byte rom[8], data[32];
int target;

SerialMonitor mon(Serial);

void setup(void) {

  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  delay(25);
  digitalWrite(3, HIGH);
  target = 0;

  Serial.begin(9600);
  
  int i;
  for (i = 0; !ds.search_exhausted() ; i++) {
    ds.reset();
    ds.find(i, rom);
    mon << "Device " << i << ", ";
    mon << ds.family_name(rom, (char*) data) << mon.endl;
    mon.printBytes(rom, 8, HEX);
    mon << ": "; 
    mon.print( ds.crc8(rom,8), HEX); 
    mon << "." << mon.endl;
        delay(1000);
  }
  if ( ds.search_exhausted() ) {
    mon << "Exhausted at " << (int)i << "." << mon.endl;
  }

    //while (true);
}


void loop(void) {
  byte i;
  byte present = 0;
  boolean result;

  ds.reset();
  result = ds.find(target, rom);
  if ( !result ) {
    mon << "Finding target " << target << " failed." << mon.endl << mon.endl;
    target = 0;
    delay(1500);
    return;
  } else {
//  mon << "Result = " << (int)result << "." << mon.endl;  
      mon << "Device " << target << ", ";
    mon << ds.family_name(rom, (char*) data) << mon.endl;
    mon.printBytes(rom, 8, HEX);
    mon << ": "; 
    mon.print( ds.crc8(rom, 8), HEX); 
    mon << "." << mon.endl;
    // next target
    target++;
  }
  delay(500);
  
  ds.reset();
  ds.select(rom);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(rom);
  ds.write(0xBE);         // Read Scratchpad
  mon << "P=";
  mon.print(present,HEX);
  mon << ", " << mon.endl;
  ds.read((byte*)data, 9);           // we need 9 bytes
  mon.print((byte*)data, 9);
  mon << " ";
  
  if (  OneWire::crc8((byte*)data, 9) ) {
    mon << "CRC error." << mon.endl;
    delay(1500);
    return;
  }

  Serial.print(0.0625*(*((int*)data)));
  Serial.print(" C");
  mon << mon.endl << mon.endl;

  delay(1500);
}


