#include <OneWire.h>
#include <DS18B20.h>
#include <SerialMonitor.h>
/*
  DS18S20 Temperature chip i/o
 */

OneWire  ds(7);  // on pin 10
byte rom[8], data[64];
int target;

SerialMonitor mon(Serial);

void setup(void) {

  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  delay(25);
  digitalWrite(3, HIGH);

  Serial.begin(19200);

  target = 0;
  //while (true);
}


void loop(void) {
  byte present = 0;
  boolean result;

  ds.reset();
  if ( !ds.find(target, rom) ) {
    mon << "\nExhausted at " << (int) target << ".\n\n";
    target = 0;
    //
    delay(3000);
    return;
  } 
  else {   
    mon << "\nDevice " << (int) target << ", ";
    mon.printBytes(rom, 8, HEX);
    if ( ds.crc8(rom,8) )
      mon << "[CRC error] ";
    mon << mon.endl;
    mon << "Type ";
    mon.print(ds.type(rom), HEX);
    mon << ", " << ds.family_name(rom, (char*) data) << mon.endl;
    target++;
  }

  if ( ds.is_temperature_sensor(rom) ) {
    ds.reset();
    ds.select(rom);
    ds.write(CONVERT_T,1);
    // start conversion, with parasite power on at the end
    delay(1000);     // maybe 750ms is enough, maybe not
    // we might do a ds.depower() here, but the reset will take care of it.

    present = ds.reset();
    ds.select(rom);
    ds.write(READ_SCRATCHPAD);         // Read Scratchpad
    /*
   mon << "P=";
     mon.print(present,HEX);
     mon << ", " << mon.endl;
     */
    ds.read((byte*)data, 9);           // we need 9 bytes
    //  mon.printByte((byte*)data, 9, HEX);

    if (  OneWire::crc8((byte*)data, 9) ) {
      mon << "CRC error." << mon.endl;
    }

    mon << Celsius(*((int*)data)) << " C\n";
  }
  //
  delay(1000);
}




