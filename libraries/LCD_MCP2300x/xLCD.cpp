//#include <LiquidCrystal.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include "xLCD.h"


xLCD::xLCD(uint8_t i2cAddr, uint8_t rs, uint8_t rw, uint8_t en,
		   uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t bklight) :
	xpander(i2cAddr)
{
	// 0b100111, 6, 0, 7, 2, 3, 4, 5, 1
	_rs_pin = rs;
	_rw_pin = rw;
	_enable_pin = en;
	
	_data_pins[0] = d0;
	_data_pins[1] = d1;
	_data_pins[2] = d2;
	_data_pins[3] = d3; 
	
	_bklight_pin = bklight;
	
    _displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
}

/*
xLCD::xLCD(MCP23009 iox, uint8_t rs,  uint8_t en,
			     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3) : 
	xLCD(iox, rs, -1, en, d0, d1, d2, d3) {
	
}
*/

void xLCD::init_xtender() {
	xpander.init();
	xpander.IOMode((byte)0x00);
	/*
	xtender.pinMode(_rs_pin, OUTPUT);
	if (_rw_pin != -1)
		xtender.pinMode(_rw_pin, OUTPUT);
	xtender.pinMode(_enable_pin, OUTPUT);
	xtender.pinMode(_data_pins[0], OUTPUT);
	xtender.pinMode(_data_pins[1], OUTPUT);
	xtender.pinMode(_data_pins[2], OUTPUT);
	xtender.pinMode(_data_pins[3], OUTPUT);
*/	//
	xpander.pullup(0xff);
	backlightOff();
}

void xLCD::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
	
	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;
	_numcolumns = cols;
//	_currline = 0;
	
	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}
	
	init_xtender();
	
	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delay(50); 
	// Now we pull both RS and R/W low to begin commands
	xpander.digitalWrite(_rs_pin, LOW);
	xpander.digitalWrite(_enable_pin, LOW);
	if (_rw_pin != 255) { 
		xpander.digitalWrite(_rw_pin, LOW);
	}
	
	//put the LCD into 4 bit or 8 bit mode
//	if (! (_displayfunction & LCD_8BITMODE)) {
		// this is according to the hitachi HD44780 datasheet
		// figure 24, pg 46
		
		// we start in 8bit mode, try to set 4 bit mode
		write4bits(0x03);
		delay(5); // wait min 4.1ms
		
		// second try
		write4bits(0x03);
		delay(5); // wait min 4.1ms
		
		// third go!
		write4bits(0x03); 
		delayMicroseconds(150);
		
		// finally, set to 8-bit interface
		write4bits(0x02); 
//	} 
/*	else {
		// this is according to the hitachi HD44780 datasheet
		// page 45 figure 23
		
		// Send function set command sequence
		command(LCD_FUNCTIONSET | _displayfunction);
		delayMicroseconds(4500);  // wait more than 4.1ms
		
		// second try
		command(LCD_FUNCTIONSET | _displayfunction);
		delayMicroseconds(150);
		
		// third go
		command(LCD_FUNCTIONSET | _displayfunction);
	}
	*/
	// finally, set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);  
	
	// clear it off
	clear();
	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
	display();
	
	// Initialize to default text direction (for romance languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);
}

size_t xLCD::write(uint8_t value) {
	send(value, HIGH);
	return 1;
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void xLCD::send(uint8_t value, uint8_t mode) {
	uint8_t myGPIO = xpander.read();
	if (mode == HIGH) {
		myGPIO |= (0x01 << _rs_pin);
	} else {
		myGPIO &= ~((uint8_t) 1 << _rs_pin);
	}
	// if there is a RW pin indicated, set it low to Write
	if (_rw_pin != -1) { 
		myGPIO &= ~((uint8_t) 1 << _rw_pin);
	}
	xpander.write(myGPIO);
	if (_displayfunction & LCD_8BITMODE) {
		// write8bits(value); 
	} else { 
		write4bits(value >> 4);
		write4bits(value);
	}
	
}

void xLCD::pulseEnable(uint8_t cache) {
	cache |= 0x01 << _enable_pin;
	xpander.write(cache);
	delayMicroseconds(1);    // enable pulse must be >450ns
	cache ^= 0x01 << _enable_pin;
	xpander.write(cache);
	delayMicroseconds(50);   // commands need > 37us to settle
}

void xLCD::pulseEnable(void) {
	pulseEnable(xpander.read());
}

void xLCD::write4bits(uint8_t value) {
	uint8_t bits = xpander.read();
	//    xtender.pinMode(_data_pins[i], OUTPUT);
	//	xtender.IOdirection(0x00);
	for (int i = 0; i < 4; i++) {
		if ( (value >> i) & 0x01 ) {
			bits |=  (uint8_t) 0x01 << _data_pins[i];
		} else {
			bits &=  ~((uint8_t)0x01 << _data_pins[i]);
		}
	}
	xpander.write(bits);
	pulseEnable(bits);
}

void xLCD::backlightOn() {
	xpander.digitalWrite(_bklight_pin, true);
}

void xLCD::backlightOff() {
	xpander.digitalWrite(_bklight_pin, false );
}

