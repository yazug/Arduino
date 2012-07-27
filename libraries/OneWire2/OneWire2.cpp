/*
Copyright (c) 2007, Jim Studt

Updated to work with arduino-0008 and to include skip() as of
2007/07/06. --RJL20

Modified to calculate the 8-bit CRC directly, avoiding the need for
the 256-byte lookup table to be loaded in RAM.  Tested in arduino-0010
-- Tom Pollard, Jan 23, 2008

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Much of the code was inspired by Derek Yerger's code, though I don't
think much of that remains.  In any event that was..
    (copyleft) 2006 by Derek Yerger - Free to distribute freely.

The CRC code was excerpted and inspired by the Dallas Semiconductor 
sample code bearing this copyright.
//---------------------------------------------------------------------------
// Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Dallas Semiconductor
// shall not be used except as stated in the Dallas Semiconductor
// Branding Policy.
//--------------------------------------------------------------------------
*/

#include "OneWire.h"

extern "C" {
#include "WConstants.h"
#include <avr/io.h>
#include "pins_arduino.h"
}


OneWire::OneWire( uint8_t pinArg)
{
  pin = pinArg;
  port = digitalPinToPort(pin);
  bitmask =  digitalPinToBitMask(pin);
  outputReg = portOutputRegister(port);
  inputReg = portInputRegister(port);
  modeReg = portModeRegister(port);
  
  exhausted = false;  
  device_no = -1;
}

//
// Perform the onewire reset function.  We will wait up to 250uS for
// the bus to come high, if it doesn't then it is broken or shorted
// and we return a 0;
//
// Returns 1 if a device asserted a presence pulse, 0 otherwise.
//
byte OneWire::reset() {
    byte r;
    byte retries = 125;

    // wait until the wire is high... just in case
    pinMode(pin,INPUT);
    do {
	if ( retries-- == 0) return 0;
	delayMicroseconds(2); 
    } while( !digitalRead( pin));
    
    digitalWrite(pin,0);   // pull low for 500uS
    pinMode(pin,OUTPUT);
    delayMicroseconds(500);
    pinMode(pin,INPUT);
    delayMicroseconds(65);
    r = !digitalRead(pin);
    delayMicroseconds(490);
    return r;
}

//
// Write a bit. Port and bit is used to cut lookup time and provide
// more certain timing.
//
void OneWire::write_bit(uint8_t v) {
    static uint8_t lowTime[] = { 55, 5 };
    static uint8_t highTime[] = { 5, 55};
    
    v = (v&1);
    *modeReg |= bitmask;  // make pin an output, do first since we
                          // expect to be at 1
    *outputReg &= ~bitmask; // zero
    delayMicroseconds(lowTime[v]);
    *outputReg |= bitmask; // one, push pin up - important for
                           // parasites, they might start in here
    delayMicroseconds(highTime[v]);
}

//
// Read a bit. Port and bit is used to cut lookup time and provide
// more certain timing.
//
uint8_t OneWire::read_bit() {
    uint8_t r;
    
    *modeReg |= bitmask;    // make pin an output, do first since we expect to be at 1
    *outputReg &= ~bitmask; // zero
    delayMicroseconds(1);
    *modeReg &= ~bitmask;     // let pin float, pull up will raise
    delayMicroseconds(5);          // A "read slot" is when 1mcs > t > 2mcs
    r = ( *inputReg & bitmask) ? 1 : 0; // check the bit
    delayMicroseconds(50);        // whole bit slot is 60-120uS, need to give some time
    
    return r;
}

//
// Write a byte. The writing code uses the active drivers to raise the
// pin high, if you need power after the write (e.g. DS18S20 in
// parasite power mode) then set 'power' to 1, otherwise the pin will
// go tri-state at the end of the write to avoid heating in a short or
// other mishap.
//
void OneWire::write(uint8_t v, uint8_t power) {
    uint8_t bitMask;
    
    for (bitMask = 0x01; bitMask; bitMask <<= 1) {
	OneWire::write_bit( (bitMask & v)?1:0);
    }
    if ( !power) {
	pinMode(pin,INPUT);
	digitalWrite(pin,0);
    }
}

//
// Read a byte
//
uint8_t OneWire::read() {
    uint8_t bitMask;
    uint8_t r = 0;
    
    for (bitMask = 0x01; bitMask; bitMask <<= 1) {
	if ( OneWire::read_bit()) r |= bitMask;
    }
    return r;
}

//
// Do a ROM select
//
void OneWire::select( byte rom[8])
{
    int i;

    write(0x55,0);         // Choose ROM

  for( i = 0; i < 8; i++) {
    write(rom[i],0);
//    Serial.print(rom[i], HEX);
//    Serial.print(" ");
  }
//  Serial.println();
}

//
// Do a ROM skip
//
void OneWire::skip() {
    write(0xCC,0);         // Skip ROM
}

void OneWire::depower()
{
    pinMode(pin,INPUT);
}

#if ONEWIRE_SEARCH

//
// You need to use this function to start a search again from the beginning.
// You do not need to do it for the first search, though you could.
//
void OneWire::search_begin(byte address[8]) {
  int i;
  last0junction = 0xff;
  exhausted = false;
  
  device_no = -1;
  for( i = 0; i < 8 ; i++) {
    address[i] = 0;
  }
}

//
// Perform a search. If this function returns a '1' then it has
// enumerated the next device and you may retrieve the ROM from the
// OneWire::address variable. If there are no devices, no further
// devices, or something horrible happens in the middle of the
// enumeration then a 0 is returned.  If a new device is found then
// its address is copied to newAddr.  Use OneWire::reset_search() to
// start over.

// This search starts from the next address of the given address.
// On reset, last0junction has been initialized to 0xff.
//
byte OneWire::search_next(byte address[8]) {
  byte b, c, r, ibyte, ibit;
  byte i;
  byte arbitraryBit = 0xff; // the -1th nonexisting bit
    
  if ( !reset() ) 
    return 0; // reset failed.
  
  write(0xf0); // SEARCH_ROM
  for( i = 0; i < 64; i++) {
    ibyte = i/8;
    ibit = i%8;
    if (address[ibyte] & 1<<ibit != 0) 
      r = 1;
    else
      r = 0;
    //
    b = read_bit();
    c = read_bit();
    if ( b && c) {
      // impossible error, something may be going worng.
      return false;
    } else if ( !b && c ) {
      bitClear(address[ibyte],ibit);
      write_bit(0);
    } else if ( b && !c ) {
      bitSet(address[ibyte],ibit);
      write_bit(1);
    } else if ( !b && !c ) {
      // if ( i < last0junction ), then leave it as is
      if ( i == last0junction ) {
        r = 1;
        bitSet(address[ibyte],ibit);
        // junction has been modified from 0 to 1
      } else if ( i > last0junction ) {
        r = 0;
        bitClear(address[ibyte],ibit);
        // junction has been forced to the initial choice 0
      }
      if ( r == 0 )
        arbitraryBit = i;
      write_bit(r);
    }
  }
  if ( arbitraryBit == 0xff ) {
    // no choice remained.
    exhausted = true;
  }
  last0junction = arbitraryBit;
  
  device_no++;
  return true;
}

byte OneWire::find(int nth, byte address[8]) {
  /*
  Serial.print("Search for ");
  Serial.print(nth, HEX);
  Serial.print(" from current device ");
  Serial.println(device_no, HEX);
  */ /*
  int i = 0;
  for (i = 0; i < 8; i++) {
    Serial.print(address[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  */
  if ( nth == device_no ) {
//    Serial.println("coincide");
    return 1;
  }
//  if ( nth < device_no ) 
    search_begin(address);

//  Serial.print("Current device is ");
//  Serial.println(device_no, HEX);

  while ( !exhausted && (device_no < nth) ) {
    if ( !search_next(address) ) 
      break;
    // search succeeded.
//    Serial.print("succeeded for ");
//    Serial.println(device_no, HEX);
  }
  return (device_no == nth);
}

#endif

#if ONEWIRE_CRC
// The 1-Wire CRC scheme is described in Maxim Application Note 27:
// "Understanding and Using Cyclic Redundancy Checks with Maxim iButton Products"
//

//
// Compute a Dallas Semiconductor 8 bit CRC directly. 
//
uint8_t OneWire::crc8(byte * addr, uint8_t len){
    uint8_t j;
    uint8_t crc = 0;
    
    for ( ; len > 0; len--) {
        uint8_t inbyte = *addr++;
        for (j = 0; j < 8; j++) {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) crc ^= 0x8C;
            inbyte >>= 1;
        }
    }
    return crc;
}

#if ONEWIRE_CRC16
static short oddparity[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };

//
// Compute a Dallas Semiconductor 16 bit CRC. I have never seen one of
// these, but here it is.
//
unsigned short OneWire::crc16(unsigned short *data, unsigned short len)
{
    unsigned short i;
    unsigned short crc = 0;
    
    for ( i = 0; i < len; i++) {
	unsigned short cdata = data[len];
	
	cdata = (cdata ^ (crc & 0xff)) & 0xff;
	crc >>= 8;
	
	if (oddparity[cdata & 0xf] ^ oddparity[cdata >> 4]) crc ^= 0xc001;
	
	cdata <<= 6;
	crc ^= cdata;
	cdata <<= 1;
	crc ^= cdata;
    }
    return crc;
}
#endif

char * OneWire::family_name(byte addr[8], char * name) {
  switch( *addr ) {
    case 0x08:
      strcpy(name, "iButton 1992: 1k memory ibutton");
      break;
    case 0x10:
      strcpy(name, "DS18S20: high precision digital thermometer");
      break;
    case 0x22:
      strcpy(name, "DS1822: Econo Digital Thermometer");
      break;
    case 0x23:
      strcpy(name, "iButton 2433: 4k eeprom");
      break;
    case 0x28:
      strcpy(name, "DS18B20: programmable resolution digital thermometer");
      break;
    case 0x01:
      strcpy(name, "24x1/iButton 1990A: silicon serial number");
      break;
    case 0x02:
      strcpy(name, "1425/iButton 1991: multikey 1153bit secure");
      break;
    case 0x33:
      strcpy(name, "2432/iButton 1961S: 1k protected eeprom with SHA-1");
      break;
    default:
      strcpy(name, "Unknown");
  }
  return name;
}

#endif
