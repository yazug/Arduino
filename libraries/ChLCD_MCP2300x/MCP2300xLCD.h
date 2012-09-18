/*
 * MCP2300xLCD.h
 *
 *  Created on: 2012/04/18
 *      Author: sin
 */

#ifndef MCP2300xLCD_H_
#define MCP2300xLCD_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WPrograms.h"
#endif

//#include "Print.h"
#include "IOExpander.h"
#include "MCP23009.h"

#include "CharacterLCD.h"

// from LiquidCrystal.h
// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class MCP2300xLCD: public CharacterLCD {
	MCP23009 xpander;
	uint8_t _rs_pin, _rw_pin, _enable_pin;
	uint8_t _data_pins[4];
	uint8_t _bklight_pin;
/*
	uint8_t _numcolumns;
	uint8_t cursorRow, cursorColumn;
*/
	void write4bits(uint8_t);
	void pulseEnable(uint8_t);
	void pulseEnable(void);

	void init_xtender();

public:
	MCP2300xLCD(uint8_t addr, uint8_t rs, uint8_t wr, uint8_t enable, uint8_t d0,
			uint8_t d1, uint8_t d2, uint8_t d3, uint8_t bklight = -1);
//	~xLCD() {}

	void reset() {
		init_xtender();
	}
	void begin(uint8_t cols = 16, uint8_t rows = 2, uint8_t dotsize =
			LCD_5x8DOTS);
	void init() {
		begin();
	}
	void send(byte value, byte dcswitch);
//	void command(uint8_t);
//#if ARDUINO >= 100
	size_t write(uint8_t);
//#else
//	void write(uint8_t);
//#endif

	const byte displayRows() {
		return _numlines;
	}
	const byte displayColumns() {
		return _numcolumns;
	}

	void backlightOn();
	void backlightOff();

};

#endif /* XLCD_H_ */
