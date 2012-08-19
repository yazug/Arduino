/*
 * Monitor.cpp
 *
 *  Created on: 2012/08/12
 *      Author: sin
 */

#include "Monitor.h"

void Monitor::printHex(const byte * a, const int length, const char gap) {
	for (int i = 0; i < length; ) {
		print(a[i]>>4, HEX);
		print(a[i]&0x0f, HEX);
		i++;
		if ( gap && i < length)
			print(gap);
	}
	return;
}

void Monitor::printHex(const char * s, const int length, const char gap) {
	for (int i = 0; i < length; ) {
		if (isprint(s[i]))
			print(s[i]);
		else {
			print("$");
			print((byte) s[i], HEX);
		}
		i++;
		if ( gap && i < length)
			print(gap);
	}
	return;
}

void Monitor::printHex(const word * a, const int length, const char gap) {
	for (int i = 0; i < length; ) {
		printHex(a[i]>>8);
		printHex(a[i]);
		i++;
		if ( gap && i < length)
			print(gap);
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

int Monitor::ithToken(const char buf[], const int item, int & fromix) {
    int tc, tend;
    for(tc = 0, tend = 0; buf[fromix] ; tc++) {
      for (fromix = tend; buf[fromix] ; fromix++)
        if ( !isspace(buf[fromix]) ) break;
      for (tend = fromix; buf[tend] ; tend++)
        if ( isspace(buf[tend]) ) break;
      if ( item == tc )
    	  break;
    }
    return tend - fromix;
}

boolean Monitor::readLine(char buf[], int maxlen, long wait) {
	long msec = millis();
	int bp = 0;
	byte c = 0;
	boolean lineEnded = false;

	buf[bp] = 0;
	while (available() && (c = read()) && bp < maxlen) {
		if (iscntrl(c) && bp != 0 ) {
			lineEnded = true;
		} else {
			msec = millis();
			buf[bp++] = c;
		}
		if ( lineEnded || (millis() > wait + msec) )
			break;
	}
	buf[bp] = 0;
	return lineEnded;
}

