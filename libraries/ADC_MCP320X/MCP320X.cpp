/*
 * MCP320X.cpp
 *
 *  Created on: 2012/05/18
 *      Author: sin
 */

#include <SPI.h>

#include "MCP320X.h"

void MCP320X::init(const byte cdiv, const byte mode) {
	pinMode(pin_cs, OUTPUT);
	digitalWrite(pin_cs, HIGH);
	setupSPI(cdiv, mode);
}

void  MCP320X::setupSPI(const byte cdiv, const byte mode) {
	clock_divider = cdiv;
	spi_mode = mode;
	setupSPI();
}

void MCP320X::setupSPI() {
	SPI.setClockDivider(clock_divider);
	SPI.setDataMode(spi_mode);
	SPI.setBitOrder(MSBFIRST);
}

word MCP320X::read12(const byte chcfg) {
	byte bh, bl;
	select();
	SPI.transfer(CONFIG_START);
	bh = SPI.transfer( chcfg<<6 | CONFIG_MSBF<<5 );
	// while accepting the first 3 bits output (SO) pin is in HI-Z state
	// (4th bit is in null low) thus the leading 3 (4th bit) bits of
	// returned values should be masked off.
	bl = SPI.transfer(0);
	deselect();
	return ((word) bh & 0x0f) << 8 | bl;
}

// read 11 bits, as either full 10 digits with a shift or the most significant 11 bits
word MCP320X::read11(const byte chcfg) {
	byte bh, bl;
	select();
	bh = SPI.transfer( CONFIG_START << 7 | chcfg << 5 | CONFIG_MSBF << 4 );
	bl = SPI.transfer(0);
	deselect();
	return ((word) bh & 0x07) << 8 | bl;
}

// read 8 bits, possibly discarding the remaining bits.
byte MCP320X::read8(const byte chcfg) {
	byte bl;
	select();
	SPI.transfer( (CONFIG_START << 4) | chcfg << 2 | CONFIG_MSBF << 1 );
	bl = SPI.transfer(0);
	deselect();
	return bl;
}

