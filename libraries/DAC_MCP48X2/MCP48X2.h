/*
 * MCP48X2.h
 *
 *  Created on: 2012/05/25
 *      Author: sin
 */

#ifndef MCP48X2_H_
#define MCP48X2_H_

#include <SPI.h>

class MCP48X2 {
	byte pin_cs;
	byte spi_mode;
	byte spi_clockdiv;

	static const byte DAC_CHANNEL = 1 << 7;
	static const byte DAC_OUTPUT_GAIN1x = 1 << 5;
	static const byte DAC_OUTPUT_SHUTDOWN = 1 << 4;

	void init(const byte cdiv, const byte mode) {
		pinMode(pin_cs, OUTPUT);
		digitalWrite(pin_cs, HIGH);
		setupSPI(cdiv, mode);
	}

public:
	MCP48X2(const byte cs = 10) : pin_cs(cs) {}

	void begin(const byte cdiv = SPI_CLOCK_DIV4, const byte mode = SPI_MODE0) { init(cdiv, mode); }
	void select() { digitalWrite(pin_cs, LOW); }
	void deselect() { digitalWrite(pin_cs, HIGH); }

	void setupSPI(const byte cdiv, const byte mode) {
		spi_clockdiv = cdiv;
		spi_mode = mode;
		setupSPI();
	}

	void setupSPI() {
		SPI.setDataMode(spi_mode);
		SPI.setClockDivider(spi_clockdiv);
		SPI.setBitOrder(MSBFIRST);
	}

	void write12(word val) {
		select();
		  SPI.transfer( (DAC_OUTPUT_GAIN1x | DAC_OUTPUT_SHUTDOWN) | (val>>8&0x0f));
		  SPI.transfer(val);
		  deselect();
	}
/*
	void write8(byte val) {
		select();
		  SPI.transfer( (DAC_OUTPUT_GAIN1x | DAC_OUTPUT_SHUTDOWN) >> 3);
		  SPI.transfer(val);
		  deselect();
	}
	*/
};

#endif /* MCP48X2_H_ */
