//#include <WProgram.h>

#include <LiquidCrystal.h>

#include "HC595iox.h"
#include "HC595LCD.h"

HC595LCD::HC595LCD(uint8_t slaveaddr, uint8_t rs, uint8_t rw, uint8_t en,
		   uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t bklight) :
	xtender(slaveaddr)
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
HC595LCD::HC595LCD(MCP23009 iox, uint8_t rs,  uint8_t en,
			     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3) : 
	HC595LCD(iox, rs, -1, en, d0, d1, d2, d3) {
	
}
*/

void HC595LCD::init_xtender() {
	xtender.init();
	backlightOff();
}

void HC595LCD::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
	
	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;
	_numcolumns = cols;
	_currline = 0;
	
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
	xtender.bitclear(_rs_pin);
	xtender.bitclear(_enable_pin);
	if (_rw_pin != 255) { 
		xtender.bitclear(_rw_pin);
	}
	
	//put the LCD into 4 bit or 8 bit mode
//	if (! (_displayfunction & LCD_8BITMODE)) {
		// this is according to the hitachi HD44780 datasheet
		// figure 24, pg 46
		
		// we start in 8bit mode, try to set 4 bit mode
		write4bits(0x03);
		delayMicroseconds(4500); // wait min 4.1ms
		
		// second try
		write4bits(0x03);
		delayMicroseconds(4500); // wait min 4.1ms
		
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


/********** high level commands, for the user! */

void HC595LCD::clear() {
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void HC595LCD::home() {
  command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void HC595LCD::setCursor(uint8_t col, uint8_t row) {
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row > _numlines ) {
    row = _numlines-1;    // we count rows starting w/0
  }
  
  command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void HC595LCD::noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
 
void HC595LCD::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void HC595LCD::noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void HC595LCD::cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void HC595LCD::noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void HC595LCD::blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void HC595LCD::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void HC595LCD::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void HC595LCD::leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void HC595LCD::rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void HC595LCD::autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void HC595LCD::noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void HC595LCD::createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    write(charmap[i]);
  }
}

/*********** mid level commands, for sending data/cmds */

void HC595LCD::command(uint8_t value) {
  send(value, LOW);
}

size_t HC595LCD::write(uint8_t value) {
  send(value, HIGH);
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void HC595LCD::send(uint8_t value, uint8_t mode) {
	uint8_t myGPIO = xtender.read();
	if (mode == HIGH) {
		myGPIO |= (0x01 << _rs_pin);
	} else {
		myGPIO &= ~((uint8_t) 1 << _rs_pin);
	}
	// if there is a RW pin indicated, set it low to Write
	if (_rw_pin != -1) { 
		myGPIO &= ~((uint8_t) 1 << _rw_pin);
	}
	xtender.write(myGPIO);
	if (_displayfunction & LCD_8BITMODE) {
		// write8bits(value); 
	} else { 
		write4bits(value >> 4);
		write4bits(value);
	}
	
}

void HC595LCD::pulseEnable(uint8_t cache) {
	cache |= 0x01 << _enable_pin;
	xtender.write(cache);
	delayMicroseconds(1);    // enable pulse must be >450ns
	cache ^= 0x01 << _enable_pin;
	xtender.write(cache);
	delayMicroseconds(50);   // commands need > 37us to settle
}

void HC595LCD::pulseEnable(void) {
	pulseEnable(xtender.read());
}

void HC595LCD::write4bits(uint8_t value) {
	uint8_t bits = xtender.read();
	//    xtender.pinMode(_data_pins[i], OUTPUT);
	//	xtender.IOdirection(0x00);
	for (int i = 0; i < 4; i++) {
		if ( (value >> i) & 0x01 ) {
			bits |=  (uint8_t) 0x01 << _data_pins[i];
		} else {
			bits &=  ~((uint8_t)0x01 << _data_pins[i]);
		}
	}
	xtender.write(bits);
	pulseEnable(bits);
}

void HC595LCD::backlightOn() {
	if ( _bklight_pin != 0xff ) {
		xtender.bitwrite(_bklight_pin, true );
	}
}

void HC595LCD::backlightOff() {
	if ( _bklight_pin != 0xff ) {
		xtender.bitwrite(_bklight_pin, false );
	}
}

