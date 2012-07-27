/*
 * MCP320X.h
 *
 *  Microchip SAR AD converter 3202/4, 3002/4
 *  Created on: 2012/05/18
 *      Author: sin
 */

#ifndef MCP320X_H_
#define MCP320X_H_

#include <Arduino.h>
#include <SPI.h>

class MCP320X {
	const byte pin_cs;
	byte clock_divider;
	byte spi_mode;

	const static byte CONFIG_START = 1;
	// in 8 bit block transfer, this comes the last bit of the first exchanging byte
	// while the receiving values are ignored
	const static byte CONFIG_MSBF = 1;
	// config bits become the first three bits of the second exchanging byte
	// and the significant four bits of data are in the receiving byte

	void init(const byte cdiv, const byte mode);

public:
	const static byte SINGLE_CH0 = 0b10;
	const static byte SINGLE_CH1 = 0b11;
	const static byte DIFFERENTIAL_CH0POS = 0b00;
	const static byte DIFFERENTIAL_CH1POS = 0b01;

	MCP320X(const byte pin = 10) : pin_cs(pin) {	}

	inline void begin(const byte cdiv = SPI_CLOCK_DIV16, const byte mode = SPI_MODE0) { init(cdiv, mode); }
	void setupSPI(const byte cdiv, const byte mode);
	void setupSPI();
	inline void end() { deselect(); }

	void select() {
		digitalWrite(pin_cs, LOW);
	}

	void deselect() {
		digitalWrite(pin_cs, HIGH);
	}

	word read12(const byte chcfig);
	word read11(const byte chcfig);
	byte read8(const byte chcfg);

};


#endif /* MCP320X_H_ */
