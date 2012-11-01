#ifndef HC595LCD_h
#define HC595LCD_h

/*
#include <inttypes.h>
#if ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif
#include <LiquidCrystal.h>
#include "Print.h"
*/
#include "CharacterLCD.h"
#include "HC595iox.h"


class HC595LCD : public Print {
	HC595iox xtender;

	uint8_t _rs_pin, _rw_pin, _enable_pin;
	uint8_t _data_pins[4];
	uint8_t _bklight_pin;
	
	uint8_t _displayfunction, _displaycontrol, _displaymode;
	uint8_t _numlines, _numcolumns;
	uint8_t _currline;

	uint8_t cursorRow, cursorColumn;

	void write4bits(uint8_t);
	void pulseEnable(uint8_t);
	void pulseEnable(void);

	void init_xtender();

public:
  HC595LCD(uint8_t slaveaddr, uint8_t rs, uint8_t wr, uint8_t enable,
		   uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t bklight = -1);
  void reset() { init_xtender(); }
  void begin(uint8_t cols = 16, uint8_t rows = 2, uint8_t dotsize = LCD_5x8DOTS);
  void init() { begin(); }
  void send(uint8_t, uint8_t);
  void command(uint8_t);
  virtual size_t write(uint8_t);
  const byte displayRows() { return _numlines; }
  const byte displayColumns() { return _numcolumns; }

  void clear();
  void home();
  void setCursor(uint8_t, uint8_t); 
	
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

  void backlightOn();
  void backlightOff();
  const boolean backlight() { return bitRead(xtender.read(), _bklight_pin); }

};

#endif
