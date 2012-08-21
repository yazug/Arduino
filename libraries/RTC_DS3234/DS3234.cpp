#include <SPI.h>
#include "DS3234.h"


PROGMEM const prog_char
	DS3234::NameOfDay[] =
		"Sun\0Mon\0Tue\0Wed\0Thu\0Fri\0Sat\0";
PROGMEM const prog_char
	DS3234::NameOfMonth[] =
		"Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0Dec\0";

long DS3234::JD2000(const long & yymmdd) {
	byte y = yymmdd>>16 & 0xff;
	byte m = yymmdd>>8 & 0xff;
	byte d = yymmdd & 0xff;
	y = 10* (y>>4&0x0f) + (y&0x0f);
	m = 10 * (m>>4&0x0f) + (m&0x0f);
	d = 10 * (d>>4&0x0f) + (d & 0x0f);
	return JD2000(y,m,d);
}

long DS3234::JD2000(byte year, byte month, byte day) {
	/* year must be after the millenium 2000 */

	if (month <= 2) {
		year--;
		month += 12;
	}
	int a = int(year / 100);
	int b = 2 - a + int(a / 4);
	return long(long(365.25 * long(year)) + int(30.6001 * (month + 1)) + day
			+ 2451479.5 + 0.5 + b); // + 0.5 for getting the day of that day's noon
//	return long(long(365.25 * long(year)) + int(30.6001 * (month + 1)) + day
//			+ 1720994.5 + b);
}

void DS3234::init() {
	pinMode(cs_pin, OUTPUT);
	digitalWrite(cs_pin, HIGH);
}

byte * DS3234::transfer(byte reg, byte * buf, int num) {
	byte * p = buf;
	select();
	SPI.transfer(reg);
	while (num-- > 0) {
		*p = SPI.transfer(*p);
		p++;
	}
	deselect();
	return buf;
}

void DS3234::setTime(const long & t) {
	transfer( ADDRESS_WRITE | REGISTER_SECONDS, (byte*) &t, 3);
	time = t;
}

void DS3234::setCalendar(const long & c) {
	// c = YYMMDD
	transfer( ADDRESS_WRITE | REGISTER_DATE, (byte*) &c, 3);
	cal = c;
}

byte DS3234::dayOfWeek() {
	return  (JD2000(cal) + 1) % 7;
}

void DS3234::updateTime() {
	transfer(REGISTER_SECONDS, (byte*) &time, 3);
	time &= ((unsigned long)BITS_HR<<16 | BITS_MIN<<8 | BITS_SEC);
//	return time;
}

void DS3234::updateCalendar() {
	transfer(REGISTER_DATE, (byte*) &cal, 3);
	cal &= ((unsigned long)BITS_YR<<16 | (unsigned long)BITS_MTH<<8 | BITS_DATE);
//	return cal;
}

void DS3234::updateTemperature() {
	byte temp[2];
	transfer(REGISTER_TEMPMSB, temp, 2);
	tempt = (char) temp[0];
	tempt <<=2;
	tempt += (temp[1]>>6);
	/*
	Serial.print("R11h ");
	Serial.print(temp[0], BIN);
	Serial.print(" ");
	Serial.print(temp[1], BIN);
	Serial.println();
	Serial.println(tempt, BIN);
	*/
	tempt *= 25;
}

