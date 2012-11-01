/*
 DS1307.h - library for DS1307/DS3231 i2c rtc
 */

// ensure this library description is only included once
#ifndef M41T62_h
#define M41T62_h

//#include <avr/pgmspace.h>

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <Wire.h>

// library interface description
class M41T62 {
	// library-accessible "private" interface
	void readRegisters(byte reg, uint8_t *, byte);
	void writeRegisters(byte reg, uint8_t *, byte);

	enum RegisterAddr {
		M41T62_100THS_SEC= 0,
		M41T62_SEC,
		M41T62_MIN,
		M41T62_HOUR,
		M41T62_DOW,
		M41T62_DATE,
		M41T62_CENT_MONTH,
		M41T62_YEAR,
		M41T62_CALIB,
		M41T62_WDMB,
		M41T62_ALARM_MONTH,
		M41T62_ALARM_DATE,
		M41T62_ALARM_HOUR,
		M41T62_ALARM_NIM,
		M41T62_ALARM_SEC,
		M41T62_WDFLAG,
	};

	static const uint8_t MT41T62_SLAVEADDR = 0b1101000;

	static const uint8_t BITS_SEC = 0b01111111;
	static const uint8_t BITS_STOPBIT = 1<<7;
	static const uint8_t BITS_MIN = 0b01111111;
	static const uint8_t BITS_OFIE = 1<<7;
	static const uint8_t BITS_HR = 0b00111111;
	static const uint8_t BITS_DOW = 0b00000111;
	static const uint8_t BITS_RS = 0b11110000;
	static const uint8_t BITS_DATE = 0b00111111;
	static const uint8_t BITS_MTH = 0b00011111;
	static const uint8_t BITS_CENT = 0b11000000;
	static const uint8_t BITS_YR = 0b11111111;
/*
	static const int DS1307_BASE_YR = 2000;
*/

//		uint8_t bcdToDec(uint8_t b) { return (b>>4)*10 + (b&0x0f); }
//		uint8_t decToBcd(uint8_t d) { return ((d/10)<<4) + (d%10); }

	// user-accessible "public" interface

public:
	PROGMEM const static prog_char NameOfDay[36];
	PROGMEM const static prog_char NameOfMonth[60];
	/*
	enum DAYINDEX {
		NA = 0, SUN = 1, MON, TUE, WED, THU, FRI, SAT,
	};
*/
public:
	long time, cal;

	M41T62() : time(0), cal(0) {
	}

	void init() {
		start();
	}
	void begin() {
		start();
	}
//		static int base_year() { return DS1307_BASE_YR; }

	void updateTime();
	void updateCalendar();
//		byte* getTimestamp(byte* );
	void setTime(const long &);
	void setCalendar(const long &);
//		long time();
//		long calendar();
//		void tweakMinutes(int);
//	void writeRegister(byte, byte);
//	byte readRegister(byte);

	void start(void);
	void stop(void);

	byte dayOfWeek();

	static long JD2000(byte y, byte m, byte d);
	static long JD2000(const long & yymmdd);

	static char * copyNameOfDay(char * buf, byte d) {
		strcpy_P(buf, NameOfDay + d * 4);
		return buf;
	}

	static char * copyNameOfMonth(char * buf, byte m) {
		strcpy_P(buf, NameOfMonth + ((m+11) % 12) * 4);
		return buf;
	}

};

#endif

