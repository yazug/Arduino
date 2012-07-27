#ifndef OneWireMaster_h
#define OneWireMaster_h

#include <avr/pgmspace.h>
#include <inttypes.h>

#include <WProgram.h>
#include <SoftwareSerial.h>

class OneWireMaster {
	
	static const uint8_t COMMAND_RESET = 0b11000001;
	static const uint8_t DATA_MODE    = 0xe1;
	static const uint8_t COMMAND_MODE = 0xe3;
	static const uint8_t SEARCH_ROM = 0xf0;
	static const uint8_t SKIP_ROM = 0xcc;
	static const uint8_t READ_ROM = 0x33;
	static const uint8_t MATCH_ROM = 0x55;
	static const uint8_t WRITE_SCRATCHPAD = 0x0f;
	static const uint8_t SEARCH_ACCELERATOR_ON = 0xb1;
	static const uint8_t SEARCH_ACCELERATOR_OFF = 0xa1;
	
	SoftwareSerial port;
	byte disbits[8];
	byte lastjunct;
	
private:
	word diffuse(byte r, byte d);
	word collate(byte b0, byte b1);
	void initializeSearchPath();
	
public:
	static byte crc8( byte *, byte);

	OneWireMaster(uint8_t rx, uint8_t tx);
	
	byte reset();
	byte configure(byte, byte);
	inline void write(byte b) { port.print(b, BYTE);}
	void write(byte *, byte);
	void dataMode() { write(DATA_MODE); }
	void commandMode() { write(COMMAND_MODE); }
	byte read() { /* while (port.available() == 0);*/ return port.read(); }
	byte search(byte addr[8]);
	void select(byte addr[8]);
};

#endif
