/*
#if ARDUINO >= 100
#include "Wire.h"
#else
extern "C" {
#include "Wire.h"
}
#endif
 */
#include "M41T62.h"
#include <Wire.h>


PROGMEM const prog_char M41T62::NameOfDay[]=
"Sun\0Mon\0Tue\0Wed\0Thu\0Fri\0Sat\0";
PROGMEM const prog_char M41T62::NameOfMonth[]=
"Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0Dec\0";

// PRIVATE FUNCTIONS

// Aquire data from the RTC chip in BCD format
// refresh the buffer
void M41T62::readRegisters(uint8_t addr, uint8_t * regvals, uint8_t num) {
	// use the Wire lib to connect to tho rtc
	// reset the resgiter pointer to zero
	Wire.beginTransmission(MT41T62_SLAVEADDR);
#if ARDUINO >= 100
//    WIRE_WRITE(addr); //Wire.send(addr);
 	Wire.write(addr);
#else
 	Wire.send(addr);
#endif
	Wire.endTransmission();

	// request num bytes of data
	Wire.requestFrom((uint8_t)MT41T62_SLAVEADDR, num);
	for(int i = 0; i < num; i++) {
		// store data in raw bcd format
#if ARDUINO >= 100
		*regvals++ = Wire.read(); //Wire.receive();
#else
		*regvals++ = Wire.receive(); //Wire.receive();
#endif
	}
}

// update the data on the IC from the bcd formatted data in the buffer
void M41T62::writeRegisters(uint8_t addr, uint8_t *regvals, uint8_t num)
{
	Wire.beginTransmission(MT41T62_SLAVEADDR);
#if ARDUINO >= 100
	Wire.write(addr); // reset register pointer
#else
	Wire.send(addr); // reset register pointer
#endif
	for(int i=0; i<num; i++) {
#if ARDUINO >= 100
		Wire.write(*regvals++);
#else
		Wire.send(*regvals++);
#endif
	}
	Wire.endTransmission();
}



void M41T62::updateTime() {
	readRegisters((byte) M41T62_SEC, (byte *) &time, 3);
	time &= ((unsigned long)BITS_HR<<16 | BITS_MIN<<8 | BITS_SEC);
}

void M41T62::updateCalendar() {
	readRegisters((byte) M41T62_DATE, (byte *) &cal, 3);
	cal &= ((unsigned long)BITS_YR<<16 | (unsigned long)BITS_MTH<<8 | BITS_DATE);
}

void M41T62::setTime(const long & p) {
//	writeRegisters((byte *) &(p ((unsigned long)BITS_HR<<16 | BITS_MIN<<8 | BITS_SEC)),
//			(byte) M41T62_SEC, 3);
	writeRegisters((byte) M41T62_SEC, (byte *) &p, 3);
}

void M41T62::setCalendar(const long & p) {
	// YYMMDD
	byte t[3];
	readRegisters((uint8_t) M41T62_DATE, (byte*) &t, 3);
	t[0] = (p & BITS_DATE) | (t[0] & ~BITS_DATE);
	t[1] = (p>>8 & BITS_MTH) | (t[1] & ~BITS_MTH);
	t[2] = p>>16 & 0xff;
	writeRegisters((uint8_t) M41T62_DATE, t, 3);
}


byte M41T62::dayOfWeek() {
	return  (JD2000(cal) + 1) % 7;
}

long M41T62::JD2000(const long & yymmdd) {
	byte y = yymmdd>>16 & 0xff;
	byte m = yymmdd>>8 & 0xff;
	byte d = yymmdd & 0xff;
	y = 10* (y>>4&0x0f) + (y&0x0f);
	m = 10 * (m>>4&0x0f) + (m&0x0f);
	d = 10 * (d>>4&0x0f) + (d & 0x0f);
	return JD2000(y,m,d);
}

long M41T62::JD2000(byte year, byte month, byte day) {
	/* year must be after the millenium 2000 */
	/*
	Serial.println(year, DEC);
	Serial.println(month, DEC);
	Serial.println(day, DEC);
	*/
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

void M41T62::stop(void)
{
	// set the ClockHalt bit high to stop the rtc
	// this bit is part of the seconds byte
//    uint8_t r;
//	readRegisters((uint8_t) M41T62_SEC, &r, 1);
//	r |= M41T62_CLOCKHALT;
 //   writeRegisters(M41T62_SEC, &r, 1);
}

void M41T62::start(void) {
	byte d[8];
	// clear stop bit, check must-be-zero bits
	readRegisters(M41T62_100THS_SEC, d, 8);
	d[1] &= 0x7f;
	d[2] &= 0x7f;
	d[3] &= 0x3f;
	d[4] &= 0x07;
	d[5] &= 0x3f;
	d[6] &= 0xdf;
	writeRegisters(M41T62_ALARM_DATE, d+1, 6);
	// alarm off, SQWE off
	readRegisters(M41T62_ALARM_MONTH, d, 4);
	d[0] = 0x1f;
	d[1] &= 0x7f;
	d[2] &= 0x7f;
	d[3] &= 0x7f;
	writeRegisters(M41T62_ALARM_MONTH, d, 4);
}
