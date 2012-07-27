/*
 Copyright (c) 2010 by arms22 (arms22 at gmail.com)
 Microchip 23x256 SPI SRAM library for Arduino

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 	 23K256/640
 	 modified by Sin

 */

#ifndef SPISRAM_H
#define SPISRAM_H

#include <SPI.h>

class SPISRAM {
private:
	const byte _ncsPin;
	byte clock_divider;
	byte spi_mode;
	byte status_cache;

	// INSTRUCTION SET
	static const byte READ  = 0x03; // Read data from memory
	static const byte WRITE = 0x02; // Write data to memory
	static const byte RDSR  = 0x05; // Read Status register
	static const byte WRSR  = 0x01; // Write Status register

	// STATUS REGISTER
	static const byte BYTE_MODE = 0x00;
	static const byte PAGE_MODE = 0x80;
	static const byte SEQ_MODE = 0x40;


	void set_access(const byte mode, const word address) {
		SPI.transfer(mode);
		SPI.transfer(address >> 8);
		SPI.transfer(address & 0xff);
	}

	void init(const byte cdiv, const byte mode);

	void writeStatusRegister(byte stat) {
		SPI.transfer(WRSR);
		SPI.transfer(stat);
	}

	byte readStatusRegister() {
		return SPI.transfer(RDSR);
	}

public:
	struct MemCell {
		SPISRAM &device;
		unsigned int address;

		MemCell &operator=(const byte data) {
			device.write(address, data);
			return *this;
		}

		operator byte() {
			return device.read(address);
		}

		MemCell(SPISRAM &d, unsigned int a) :
				device(d), address(a) {
		}
		;
		~MemCell() {
		}
		;
	};
	MemCell operator[](unsigned int address) {
		MemCell memcell(*this, address);
		return memcell;
	}

	SPISRAM(byte ncsPin);

	inline void begin(const byte cdiv = SPI_CLOCK_DIV8, const byte mode = SPI_MODE0) { init(cdiv, mode); }
	void setupSPI(const byte cdiv, const byte mode);
	void setupSPI();

	byte read(unsigned int address);
	void read(unsigned int address, byte *buffer, unsigned int size);
	void write(unsigned int address, byte data);
	void write(unsigned int address, byte *buffer, unsigned int size);

	void select(void);
	void deselect(void);
};

#endif
