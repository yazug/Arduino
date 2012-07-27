/*
 * MCP4725.h
 *
 *  Created on: 2012/05/23
 *      Author: sin
 */

#ifndef MCP4725_H_
#define MCP4725_H_

#include <Arduino.h>

class MCP4725 {
	const static byte DEVICE_ADDRESS = 0b1100 << 3 | 0b000; // 7 bit address
	const static byte FAST_MODE = 0b00 << 6;
	const static byte POWER_DOWN_NORMAL = 0b00 << 4;

public:
	MCP4725() {	}
	void begin() {}

	void write(word val) {
	  Wire.beginTransmission(DEVICE_ADDRESS);
	  Wire.write(FAST_MODE | POWER_DOWN_NORMAL | (val >>8 & 0x0f) );
	  Wire.write(val & 0xff);
	  Wire.endTransmission();
	}
};


#endif /* MCP4725_H_ */
