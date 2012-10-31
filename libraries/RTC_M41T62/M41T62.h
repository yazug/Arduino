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
public:
	void readRegisters(byte reg, uint8_t *, byte);
	void writeRegisters(byte reg, uint8_t *, byte);

	enum {
		M41T62_SEC = 0,
		M41T62_MIN,
		M41T62_HR,
		M41T62_DOW,
		M41T62_DATE,
		M41T62_MTH,
		M41T62_YR
	};

	static const uint8_t MT41T62_SLAVEADDR = 0b1101000;
/*
	// Define register bit masks
	static const uint8_t M41T62_CLOCKHALT = 0b10000000;
*/
	static const uint8_t BITS_SEC = 0b01111111;
	static const uint8_t BITS_HR = 0b00111111;
	static const uint8_t BITS_MIN = 0b01111111;
	static const uint8_t BITS_DOW = 0b00000111;
	static const uint8_t BITS_DATE = 0b00111111;
	static const uint8_t BITS_MTH = 0b00111111;
	static const uint8_t BITS_YR = 0b11111111;
/*
	static const int DS1307_BASE_YR = 2000;
*/

//		uint8_t bcdToDec(uint8_t b) { return (b>>4)*10 + (b&0x0f); }
//		uint8_t decToBcd(uint8_t d) { return ((d/10)<<4) + (d%10); }

	// user-accessible "public" interface

public:
//	PROGMEM const static prog_char NameOfDay[36];
//	PROGMEM const static prog_char NameOfMonth[60];
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
/*
	byte dayOfWeek();
	*/
	static long JD2000(byte y, byte m, byte d);
	static long JD2000(const long & yymmdd);
/*
	static char * copyNameOfDay(char * buf, byte d) {
		strcpy_P(buf, NameOfDay + d * 4);
		return buf;
	}

	static char * copyNameOfMonth(char * buf, byte m) {
		strcpy_P(buf, NameOfMonth + ((m+11) % 12) * 4);
		return buf;
	}
*/
};

#endif

