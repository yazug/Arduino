/*
 * Monitor.cpp
 *
 *  Created on: 2012/08/12
 *      Author: sin
 */

#include "Monitor.h"

void Monitor::printHex(byte * a, const int length) {
	for (int i = 0; i < length; i++) {
		print(*a >> 4, HEX);
		print(*a & 0x0f, HEX);
		print(' ');
		a++;
	}
	return;
}

void Monitor::printHex(const char * s, const int length) {
	for (int i = 0; i < length; i++) {
		if (isprint(s[i]))
			print(s[i]);
		else {
			print("$");
			print((byte) s[i], HEX);
		}
	}
	return;
}

void Monitor::printHex(const word * a, const int length) {
	for (int i = 0; i < length; i++) {
		print(a[i] >> 12, HEX);
		print(a[i] >> 8 & 0x0f, HEX);
		print(a[i] >> 4 & 0x0f, HEX);
		print(a[i] & 0x0f, HEX);
		print(' ');
	}
	return;
}
/*
 const char * printHex(const byte b) {
 sprintf(sbuf, "%02X", b);
 return sbuf;
 }

 const char * printHex(const word w) {
 sprintf(sbuf, "%04X", w);
 return sbuf;
 }
 */

word Monitor::readToken(char buf[], long timeout) {
	long msec = millis();
	int bp = 0;
	byte c = 0;
	buf[bp] = 0;

	while (available() && (c = read())) {
		if (isspace(c)) {
			if (bp != 0)
				break;
		} else {
			msec = millis();
		}
		buf[bp++] = c;
		while (!available() && (millis() < timeout + msec))
			;
		if (!available())
			break;
	}
	buf[bp] = 0;
	return bp;
}
