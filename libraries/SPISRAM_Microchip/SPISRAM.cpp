/*
 Copyright (c) 2010 by arms22 (arms22 at gmail.com)
 Microchip 23x256 SPI SRAM library for Arduino

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <Arduino.h>
#include <SPI.h>
#include "SPISRAM.h"

SPISRAM::SPISRAM(byte ncsPin) :
		_ncsPin(ncsPin) {
}

void SPISRAM::init() {
	pinMode(_ncsPin, OUTPUT);
	digitalWrite(_ncsPin, HIGH);
	//
	select();
	writeStatusRegister(SEQ_MODE);
	deselect();
}

byte SPISRAM::read(const word & address) {
	byte data;
	select();
	set_access(READ, address);
	data = SPI.transfer(0);
	deselect();
	return data;
}

byte * SPISRAM::read(const word & address, byte *buffer, const word & size) {
	select();
//	SPI.transfer(WRSR);
//	SPI.transfer(SEQ_MODE);
	//
	byte * p = buffer;
	set_access(READ, address);
	for (unsigned int i = 0; i < size; i++)
		*p++ = SPI.transfer(0);
	deselect();
	return buffer;
}

void SPISRAM::write(const word & address, byte data) {
	select();
	set_access(WRITE, address);
	SPI.transfer(data);
	deselect();
}

void SPISRAM::write(const word & address, byte *buffer, const word & size) {
	select();
//	SPI.transfer(WRSR);
//	SPI.transfer(SEQ_MODE);
	//
	set_access(WRITE, address);
	for (unsigned int i = 0; i < size; i++)
		SPI.transfer(*buffer++);
	deselect();
}

void SPISRAM::setSPIMode(void) {
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	SPI.setDataMode(SPI_MODE0);
}

void SPISRAM::csLow() {
	digitalWrite(_ncsPin, LOW);
}

void SPISRAM::csHigh() {
	digitalWrite(_ncsPin, HIGH);
}

void SPISRAM::select(void) {
	setSPIMode();
//	Serial.print(_ncsPin, HEX);
//	Serial.print(" = ");
	digitalWrite(_ncsPin, LOW);
//	Serial.println(digitalRead(_ncsPin), HEX);
}

void SPISRAM::deselect(void) {
//	Serial.print(_ncsPin, HEX);
//	Serial.print(" = ");
	digitalWrite(_ncsPin, HIGH);
//	Serial.println(digitalRead(_ncsPin), HEX);
}
