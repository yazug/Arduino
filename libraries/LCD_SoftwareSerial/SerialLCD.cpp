/* 
 * by sin
 */

#ifdef SoftwareSerial_h
#include <SoftwareSerial.h>
#endif
//#include "LCD.h"
#include "SerialLCD.h"

/* ======================================================== */

//--------------------------

void SerialLCD::begin(long speed, uint8_t cols, uint8_t rows, uint8_t charsize) {
	switch (speed) {
		case 2400:
		case 4800:
		case 9600:
		case 14400:
		case 19200:
		case 38400:
//			baud = speed;
			break;
		default:
			speed = 9600;
			break;
	}
	if (sertype == HARDWARESERIAL)
		((HardwareSerial&) serport).begin(speed);
//#ifdef SoftwareSerial
	else if ( sertype == SOFTWARESERIAL )
		((SoftwareSerial&)serport).begin(speed);
//#endif
	init(cols, rows, charsize);
}

void SerialLCD::init(uint8_t cols, uint8_t lines, uint8_t dotsize) {
	_displayfunction = LCD_4BITMODE; // | LCD_1LINE; // | LCD_5x8DOTS;
	_numlines = 1;
	bounceDelay = 4;

	delay(bounceDelay);
	_numcolumns = cols;
	_numlines = lines;
	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	} else {
		_displayfunction |= LCD_1LINE;
	}
	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	currow = 0;
	curcol = 0;

	// finally, set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);

	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();

	// clear it off
	clear();

	// Initialize to default text direction (for romance languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);

	//------
	backlightOn();
	noCursor();
}

//--------------------------

/*********** mid level commands, for sending data/cmds */
/*
void SerialLCD::command(uint8_t value) {
	send(value, LOW);
}
*/
size_t SerialLCD::write(uint8_t value) {
	if (value == '\r' ){
	} else if ( value == '\n') {
		for( ; curcol < _numcolumns; curcol++) {
			send(' ', HIGH);
		}
		curcol = 0;
		currow = (currow + 1) % _numlines;
		setCursor(curcol, currow);
	} else {
		curcol++;
		if (curcol > _numcolumns) {
			curcol = 0;
			currow = (currow+1) % _numlines;
		}
		send(value, HIGH);
	}
	return 1;
}
/*
size_t SerLCD::write(uint8_t * str) {
	return send(str, HIGH);
}
*/
/********** high level commands, for the user! */

void SerialLCD::clear()
{
	command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
	currow = 0;
	curcol = 0;
	delay(5);  // this command takes a long time!
}

void SerialLCD::home()
{
	
	command(LCD_RETURNHOME);  // set cursor position to zero
	 currow = 0;
	 curcol = 0;
	delay(5);  // this command takes a long time!
	
	//setCursor(0, 0);
}

void SerialLCD::setCursor(uint8_t col, uint8_t row)
{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	curcol = col;
	currow = row % _numlines;
	command(LCD_SETDDRAMADDR | (curcol + row_offsets[currow]));
}



//--------------------------
void SerialLCD::brightness ( int brt ) {
	brt = max(0,brt);
	brt = min(brt,29);
	send(SPECIAL_COMMAND_CHAR, LOW);
	delay(200); 
	send(128+brt, LOW);
	delay(200); 
}


/************ low level data pushing commands **********/

void SerialLCD::send(uint8_t c, uint8_t mode) {
	if (mode == HIGH) {
		serport.write((uint8_t)c);
	} else {
		serport.write((uint8_t)EXTENDED_COMMAND_CHAR);
		delay(bounceDelay);
		serport.write((uint8_t) c);
		delay(bounceDelay);
	}
}
