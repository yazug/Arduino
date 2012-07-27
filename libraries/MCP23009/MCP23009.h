#ifndef MCP23009_h
#define MCP23009_h

#include <inttypes.h>
#include "IOExpander.h"

class MCP23009 : public IOExpander {
	enum { // constant
		IODIR = 0x00,
		IPOL = 0x01,
		IOCON = 0x05, 
		GPPU = 0x06,
		GPIO = 0x09,
		OLAT = 0x0a,

		DEFAULTADDR = 0b100111,

		IODIR_INPUT = 0x01,
		IODIR_OUTPUT = 0x00
	};
	uint8_t address;
	uint8_t comm_result;

public:
	MCP23009(const uint8_t addr = DEFAULTADDR);	
//	MCP23009(void) { MCP23009(DEFAULTADDR); }
	void init();

//	~MCP23009() {}

    byte send(const byte reg, const byte val);
	byte receive(const byte reg);

public:
	inline
	byte read(const byte reg) { return receive(reg); }
	byte read() { return read(GPIO); }
	inline
	byte write(const byte val) { return send(GPIO, val); }
	byte write(const byte addr, const byte val) { return send(addr, val); }
	byte transfer(const byte val) {
		byte res = read();
		write(val);
		return res;
	}
//	uint8_t gpio() { return read(GPIO); }
	byte IOMode() { return read(IODIR); }
	byte IOMode(const byte val) { return send(IODIR, val); }
	void polarity(byte val) { send(IPOL, val); }
	void config(byte val) { send(IOCON, val); }
	byte pullup(const byte val) { return send(GPPU, val); }

	void pinMode(byte pin, byte ArduinoIOmode);

};

#endif
