#ifndef DS3234_H
#define DS3234_H

#include <avr/pgmspace.h>

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <SPI.h>

class DS3234 {
	byte cs_pin;

	//	static const uint8_t DS1307_CLOCKHALT = 0b10000000;

	static const uint8_t BITS_SEC = 0b01111111;
	static const uint8_t BITS_HR = 0b00111111;
	static const uint8_t BITS_MIN = 0b01111111;
	static const uint8_t BITS_DOW = 0b00000111;
	static const uint8_t BITS_DATE = 0b00111111;
	static const uint8_t BITS_MTH = 0b00111111;
	static const uint8_t BITS_YR = 0b11111111;

//	static const int DS1307_BASE_YR = 2000;

	static const byte ADDRESS_READ = 0x00;
	static const byte ADDRESS_WRITE = 0x80;

	// CONTROL_REGISTER_BIT
	enum {
		A1IE = 0,
		A2IE = 1,
		INTCN = 2,
		RS1 = 3,
		CONV = 5,  // do temp conversions to adjust clock
		BBSQW = 6,
		_EOSC = 7  // stop clock while battery powered
	};
	// CONTROL_STATUS_REGISTER_BIT
	enum  {
		OSF = 7,
		BB32kHz = 6,
		CRATE0 = 4,
		EN32kHz = 3,
		BSY = 2,
		A2F = 1,
		A1F = 0
	};
	static const byte DEFAULT_REGISTER_CONTROL = 0x00 | (1 << CONV) | (1<<INTCN);
	static const byte DEFAULT_REGISTER_CONTROL_STATUS = 0x00 ; // all-off
	//osf must be cleared, bb32 should be cleared, en32 should be cleared

	byte * transfer(byte reg, byte * buf, int num);

public:
	static const byte REGISTER_SECONDS = 0x00;
	static const byte REGISTER_DAY = 0x03;
	static const byte REGISTER_DATE = 0x04;
	static const byte REGISTER_MONTH_CENTURY = 0x05;
	static const byte REGISTER_YEAR = 0x06;
	static const byte REGISTER_TEMPMSB = 0x11;
	static const byte REGISTER_TEMPLSB = 0x12;
	static const byte REGISTER_CONTROL = 0x0E;
	static const byte REGISTER_CONTROL_STATUS = 0x0F;

	PROGMEM const static prog_char NameOfDay[36];
	PROGMEM const static prog_char NameOfMonth[60];
	enum DAYINDEX {
		NA = 0, SUN = 1, MON, TUE, WED, THU, FRI, SAT,
	};

public:
	long time, cal;
	int tempt;

	DS3234(byte pin) : cs_pin(pin), time(0), cal(0), tempt(0) {
//		deselect();
	}

	boolean init();
	inline boolean begin() {
		return init();
	}

	void setSPIMode(void) {
		SPI.setClockDivider(SPI_CLOCK_DIV8);
		SPI.setDataMode(SPI_MODE1);
		SPI.setBitOrder(MSBFIRST);
	}

	void select() {
		SPI.setClockDivider(SPI_CLOCK_DIV8);
		SPI.setDataMode(SPI_MODE1);
		SPI.setBitOrder(MSBFIRST);
		csLow();
	}
	void deselect() {
		csHigh();
	}

	inline void csLow() {
		digitalWrite(cs_pin, LOW);
	}
	inline void csHigh() {
		digitalWrite(cs_pin, HIGH);
	}

	static long JD2000(byte y, byte m, byte d);
	static long JD2000(const long & yymmdd);

	static char * copyNameOfDay(char * buf, byte d) {
		strcpy_P(buf, NameOfDay + d * 4);
		return buf;
	}

	static char * copyNameOfMonth(char * buf, byte m) {
		strcpy_P(buf, NameOfMonth + ((m + 11) % 12) * 4);
		return buf;
	}

	void setCalendar(const long & yymmdd);
	inline void setCalendar() { setCalendar(cal); }
	void setTime(const long & hhmmdd);
	inline void setTime() { setTime(time); }
	void updateTime();
	void updateCalendar();
	void update() {
		updateTime();
		updateCalendar();
	}
	void updateTemperature(); // returns x100 of Celsius Centigrade.

	inline void writeRegister(byte r, byte val) {
		transfer(ADDRESS_WRITE | r, (byte*)&val, 1);
	}

	inline byte readRegister(byte r) {
		byte t;
		transfer(ADDRESS_READ | r, (byte*)&t, 1);
		return t;
	}

	byte dayOfWeek();

};

#endif //DS3234_H
