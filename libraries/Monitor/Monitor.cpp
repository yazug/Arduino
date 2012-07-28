/*
 *  Monitor.cpp
 */

#include "Monitor.h"

/*
char * Monitor::byteString(char * buf, const byte val[], const int len,
		const byte base) {
	char * p = buf;
	for (int i = 0; i < len; i++) {
		switch (base) {
		case HEX:
			p += sprintf(p, "%02X ", val[i]);
			break;
		case BIN:
			if (i > 0)
				p += sprintf(p, " ");
			for (int bit = 0; bit < 8; bit++)
				p += sprintf(p, "%d", val[i] >> bit & 1);
			break;
		}
	}
	return buf;
}
*/

void Monitor::printNumber(byte n, byte base) {
	byte buf[8 * sizeof(long)]; // Assumes 8-bit chars.
	unsigned long i = 0;
	int d = 255;

	if (base == 255) {
		if (isprint(n)) {
			print((char) n);
			print(' ');
		} else {
			print("\\x");
			print(n >> 4 & 0x0f, base);
			print(n & 0x0f, base);
		}
	} else {

		while (d > 0) {
			buf[i++] = n % base;
			n /= base;
			d /= base;
		}

		for (; i > 0; i--)
			print(
					(char) (buf[i - 1] < 10 ?
							'0' + buf[i - 1] : 'A' + buf[i - 1] - 10));
	}
}


unsigned long Monitor::asBCD(unsigned long lval) {
	unsigned long tmp;
	byte * p = (byte*) (&tmp);
	int i;
	for (i = 0; i < 4; i++) {
		p[i] = lval % 10;
		lval /= 10;
		p[i] |= (lval % 10) << 4;
		lval /= 10;
	}
	return tmp;
}

int Monitor::asHexadecimal(char * str, int limit) {
	boolean lowerNibble = true;
	byte * readptr = (byte*) str;
	byte b;
	int count = 0;
	for (; ((b = *readptr) != (char) 0) && (limit == 0 ? true : count < limit);
			readptr++) {
		if (isspace(b))
			break;
		if (isdigit(b)) {
			b -= '0';
		} else if (isxdigit(b)) {
			b = toupper(b) - 'A' + 0x0a;
		} else {
			break;
		}
		if (lowerNibble) {
			*str = b;
			lowerNibble = false;
		} else {
			*str = ((*str) << 4) | b;
			str++;
			lowerNibble = true;
			count++;
		}
	}
	return count;
}

const Monitor & Monitor::printHexString(const word array[], const int size) {
	int i;
	for (i = 0; i < size; i++) {
		if ( i > 0 )
			print(' ');
		print(array[i]>>12 & 0x0f, curr_base);
		print(array[i]>>8 & 0x0f, curr_base);
		print(array[i]>>4 & 0x0f, curr_base);
		print(array[i]&0x0f, curr_base);
	}
	return *this;
}

const Monitor & Monitor::printHexString(const byte array[], const int size) {
	int i;
	for (i = 0; i < size; i++) {
		if ( i > 0 )
			print(' ');
		print(array[i]>>4, curr_base);
		print(array[i]&0x0f, curr_base);
	}
	return *this;
}

int Monitor::read(byte rcv[], const int maxlen, const long limit) {
	int i = 0;
	int c;
	long current = millis();

	while ( i < maxlen && millis() < current + limit ) {
		if ( available() && (c = read()) != -1 ) {
			if ( rcv != NULL )
				rcv[i] = (byte) c;
			i++;
			current = millis();
		}
	}
	return i;
}

// wait a specific string in stream
boolean Monitor::wait(const byte msg[], const int length, const long limit) {
	int i = 0;
	byte c;
	long current = millis();
	while ( i < length && millis() < current + limit ) {
		if ( available() && (c = read()) != -1 ) {
			if ( msg[i] != (byte) c )
				return false;
			i++;
			current = millis();
		}
	}
	if ( i < length )
		return false;
	return true;
}


int Monitor::readToken(char buf[], long timeout) {
	long msec = millis();
	int bp = 0;
	byte c = 0;
	buf[bp] = 0;

	while (!available() && (millis() < timeout + msec))
		;
	while (available() && (c = read())) {
		if (isspace(c) ) {
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

int Monitor::readNumber(char buf[], long timeout) {
	long msec = millis();
	int bp = 0;
	byte c = 0;
	buf[bp] = 0;

	while (available() && (c = read())) {
		if ( !(isdigit(c) || c == '.' || c == '-' ) ) {
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
