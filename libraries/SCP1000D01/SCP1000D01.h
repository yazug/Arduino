/**
 * SCP1000 Library
 *
 *
 * Manages communication with SCP1000 barometric pressure & temperature sensor
 *
 */
#ifndef SCP1000D01_h
#define SCP1000D01_h

#include "WProgram.h"


class SCP1000
{
public:
	// Class global constants
	const static byte DataOutPin = 11;
	const static byte DataInPin = 12;
	const static byte SPIClockPin = 13;

	// Register Addresses
	const static byte REVID = 0x00;		// ASIC Revision Number
	const static byte OPREG = 0x03;		// Operation Register
	const static byte OPSTATUS = 0x04;		// Operation Status
	const static byte STATUS = 0x07;		// ASIC Status
	const static byte PRESSURE = 0x1F;		// Pressure 3 MSB
	const static byte PRESSURE_LSB = 0x20;	// Pressure 16 LSB
	const static byte TEMP = 0x21;			// 16 bit temp
	const static byte RSTR = 0x06;			// Soft Reset register
	
	// Mode values
	const static byte HiResMode = 0x0A;	// Hi Resolution, Constant Readings
	const static byte StandbyMode = 0x00;	// No operation

	const static byte STATUS_STARTUP = 0x01;
	
public:
	
	SCP1000(const byte csPin, const byte DRDYPin = -1);
	void init();
//	void read();
	long pressure();
	long temperature();
	void reset();
	void standby();
	void run();
	byte revision() { return read_register(REVID); }
	byte status() { return read_register(OPSTATUS); }
	byte startup() { return STATUS_STARTUP & read_register(OPSTATUS); }
	
	
private:
	// I/O Pins
	byte _selectPin;
	byte _DRDYPin;
	
	void readPressure();
	void readTemperature();
	unsigned int read_register(byte register_name, byte numBytes = 1);
	void write_register(byte register_name, byte data);
//	byte spi_transfer(volatile byte data);
};

#endif


