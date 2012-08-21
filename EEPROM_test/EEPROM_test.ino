#include <EEPROM.h>

#include "Monitor.h"

struct Date {
  word year;
  byte month, day;
  byte xsum;
};

Monitor mon(Serial);

void setup() {
  Date today = {2011, 3, 20, 0xff};
  Date anotherday  = {1976, 5, 27, 0xff};
  byte chksum;
  Serial.begin(19200);
  mon << endl 
    << today.year << "/" << (int) today.month << "/" << (int) today.day << 
    endl << "check sum: " << (int) today.xsum << endl;
  chksum = write_date(anotherday);
  mon << "chk: " << (int) chksum << endl;
  chksum = read_date(today);
  mon << endl 
    << today.year << "/" << (int) today.month << "/" << (int) today.day << 
    endl << "check sum: " << (int) chksum << endl;
}

void loop() {
}

byte read_date(struct Date & d ) {
  byte * ptr = (byte*)&d;
  byte c = 0;
  for(int addr = 0; addr < sizeof(Date); addr++, ptr++) {
    *ptr = EEPROM.read(addr);
    c ^= *ptr;
    mon << (int)c << endl;
  }
  return c;
}

byte write_date(struct Date d ) {
  byte * ptr = (byte*)&d;
  byte c = 0;
  int addr;
  for(addr = 0; addr < sizeof(Date) - 1; addr++, ptr++) {
    EEPROM.write(addr,*ptr);
    c ^= *ptr;
  }
  EEPROM.write(addr,c);
  return c;
}


