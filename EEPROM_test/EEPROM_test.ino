#include <EEPROM.h>
#include "Monitor.h"

Monitor mon(Serial);

void setup() {
  Serial.begin(9600);
  mon << "writing to EEPROM..." << endl;
  //
  byte mac[] = { 0x00, 0x50, 0xc2, 0x97, 0x21, 0x73 };
  byte ip[] = {192, 168, 1, 177};
  byte mask[] = { 255, 255, 255, 0 };
  byte gtway[] = { 192, 168, 1, 1 };
  byte magic[] = { 0x9e, 'I' };
  //
  int addr = 0;
  addr += EEPROM_write(addr, magic, 2);
  addr += EEPROM_write(addr, mac, 6);
  addr += EEPROM_write(addr, ip, 4);
  addr += EEPROM_write(addr, mask, 4);
  addr += EEPROM_write(addr, gtway, 4);
  //
  byte buf[8];
  addr = 0;
  addr += EEPROM_read(addr, buf, 2);
  mon.printBytes(buf, 2);
  mon << endl;
  addr += EEPROM_read(addr, buf, 6);
  mon.printBytes(buf, 6);
  mon << endl;
  addr += EEPROM_read(addr, buf, 4);
  mon.printBytes(buf, 4, '.', DEC);
  mon << endl;
  addr += EEPROM_read(addr, buf, 4);
  mon.printBytes(buf, 4, '.', DEC);
  mon << endl;
  addr += EEPROM_read(addr, buf, 4);
  mon.printBytes(buf, 4, '.', DEC);
  mon << endl;
}

void loop() {
}

int EEPROM_read(int addr, byte * ptr, int n) {
  for(int i = 0 ; i < n; i++, ptr++) {
    *ptr = EEPROM.read(addr+i);
  }
  return n;
}

int EEPROM_write(int addr, byte * ptr, int n) {
  for(int i = 0 ; i < n; i++, ptr++) {
    EEPROM.write(addr+i, *ptr);
  }
  return n;
}


