/*
 */
//#include <inttypes.h>
#if ARDUINO >= 100
  #include "Arduino.h"
#else
   #include "WPrograms.h"
#endif
#include <Wire.h>
//#include "PortExpander.h"
#include "MCP23009.h"

#ifdef ARDUINO >= 100
	inline byte Wsend(byte v) { return Wire.write(v); }
	inline byte Wreceive() { return Wire.read(); }
#else
	inline byte Wsend(byte v) { return Wire.send(v); }
	inline byte Wreceive() { return Wire.receive(); }
#endif


MCP23009::MCP23009(uint8_t addr) {
	address = (DEFAULTADDR & 0b01111000) | (addr & 0x07);
	comm_result = 0;
}

void MCP23009::init() {
	// the same as Power-On/Reset value
	IOMode(0xff); // set IODIR, 0x00 = all output
	polarity(0x00);
	config(0x00); // set IOCON w/ non-increment register address mode
	pullup(0x00); // set no pull-ups
}

uint8_t MCP23009::send(const uint8_t reg, const uint8_t val) {
  Wire.beginTransmission(address);
  Wsend(reg); //Wire.send(reg);
  Wsend(val); //Wire.send(val);
  return comm_result = Wire.endTransmission();
}

// returns received value, set result in this->comm_result.
uint8_t MCP23009::receive(const uint8_t reg) {
  Wire.beginTransmission(address);
  Wsend(reg); //Wire.send(reg);
  comm_result = Wire.endTransmission();

  comm_result = Wire.requestFrom(address,(uint8_t) 1);
  while (Wire.available() < 1) ;
  return Wreceive(); //Wire.receive();
}

void MCP23009::pinMode(byte pin, byte mode) {
	pin &= 0x07;
	byte dir = IOMode();
	IOMode( bitWrite(dir,pin,(mode == OUTPUT? IODIR_OUTPUT : IODIR_INPUT)  ) );
}
