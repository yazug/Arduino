/*
 Copyright (c) 2010 by arms22 (arms22 at gmail.com)
 Microchip 23x256 SPI SRAM library for Arduino

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <SPI.h>
#include "SPISRAM.h"

SPISRAM::SPISRAM(byte ncsPin) :
		_ncsPin(ncsPin) {
}

void SPISRAM::init(const byte cdiv, const byte mode) {
	pinMode(_ncsPin, OUTPUT);
	digitalWrite(_ncsPin, HIGH);
	setupSPI(cdiv, mode);
	//
	select();
	writeStatusRegister(SEQ_MODE);
	deselect();
}

void SPISRAM::setupSPI() {
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(clock_divider);
	SPI.setDataMode(spi_mode);
}
void SPISRAM::setupSPI(const byte cdiv, const byte mode) {
	clock_divider = cdiv;
	spi_mode = mode;
	setupSPI();
}

byte SPISRAM::read(unsigned int address) {
	byte data;
	select();
	set_access(READ, address);
	data = SPI.transfer(0);
	deselect();
	return data;
}

void SPISRAM::read(unsigned int address, byte *buffer, unsigned int size) {
	select();

//	SPI.transfer(WRSR);
//	SPI.transfer(SEQ_MODE);
	//
	set_access(READ, address);
	for (unsigned int i = 0; i < size; i++) {
		*buffer++ = SPI.transfer(0);
	}
	deselect();
}

void SPISRAM::write(unsigned int address, byte data) {
	select();
	set_access(WRITE, address);
	SPI.transfer(data);
	deselect();
}

void SPISRAM::write(unsigned int address, byte *buffer, unsigned int size) {
	select();
//	SPI.transfer(WRSR);
//	SPI.transfer(SEQ_MODE);
	//
	set_access(WRITE, address);
	for (unsigned int i = 0; i < size; i++) {
		SPI.transfer(*buffer++);
	}
	deselect();
}

void SPISRAM::select(void) {
	digitalWrite(_ncsPin, LOW);
}

void SPISRAM::deselect(void) {
	digitalWrite(_ncsPin, HIGH);
}
