/* 
 * by sin
 */

#ifndef SerialLCD_h
#define SerialLCD_h

/*
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <Print.h>
*/
#include <SoftwareSerial.h>

#include "CharacterLCD.h"

class SerialLCD: public CharacterLCD {
	static const uint8_t EXTENDED_COMMAND_CHAR = 0xFE;
	static const uint8_t SPECIAL_COMMAND_CHAR = 0x7C;

	Stream & serport;
	const enum {
		UNDEFINED = 0, HARDWARESERIAL, SOFTWARESERIAL,
	} sertype;

//	uint8_t lastBright;
public:
	uint8_t currow, curcol;
private:
	int bounceDelay;
	uint8_t _displayfunction;
	uint8_t _displaycontrol;
	uint8_t _displaymode;
	uint8_t _initialized;
	uint8_t _numlines, _numcolumns;

public:
	SerialLCD(HardwareSerial & stream) :
		serport(stream), sertype(HARDWARESERIAL) {
	}

	SerialLCD(SoftwareSerial & stream) :
		serport(stream), sertype(SOFTWARESERIAL) {
	}

	void init(uint8_t cols, uint8_t rows, uint8_t charsize);
	void begin(long speed, uint8_t cols = 16, uint8_t rows = 2,
			uint8_t charsize = LCD_5x8DOTS);

	virtual void send(uint8_t, uint8_t);
//	virtual void command(uint8_t value);
	virtual size_t write(uint8_t value);

	void clear();
	void home();
	void setCursor(uint8_t, uint8_t);
	uint8_t cursorRow() { return currow; }
	uint8_t cursorColumn() { return curcol; }

	void brightness(int level);
	void backlightOn(int val = 100) {
		brightness(val * 10 / 34);
	}
	void backlightOff() {
		brightness(0);
	}
	/*
	 void noDisplay();
	 void display();
	 void noBlink();
	 void blink();
	 void noCursor();
	 void cursor();
	 void scrollDisplayLeft();
	 void scrollDisplayRight();
	 void leftToRight();
	 void rightToLeft();
	 void autoscroll();
	 void noAutoscroll();

	 void createChar(uint8_t, uint8_t[]);
	 */
};

#endif
