/*
 * Monitor.h
 *
 *  Created on: 2012/08/07
 *      Author: sin
 */

#ifndef MONITOR_H_
#define MONITOR_H_

#include <Arduino.h>
#include <Stream.h>

const char endl = '\n';

template<class T>
inline Stream &operator <<(Stream &stream, T arg) {
	stream.print(arg);
	return stream;
}

class Monitor {
	char * sbuf;

public:
	Monitor(char * buf) :
			sbuf(buf) {
	}

	const char * printHex(const byte * a, const int length) {
		char * p = sbuf;
		for (int i = 0; i < length; i++) {
			sprintf(p, "%02X ", a[i]);
			p += 3;
		}
		return sbuf;
	}

	const char * printHex(const char * s, const int length) {
		char * p = sbuf;
		for (int i = 0; i < length; i++) {
			if (isprint(s[i]))
				sprintf(p, "%c ", s[i]);
			else
				sprintf(p, "\\x%02x ", s[i]);
			p += 3;
		}
		return sbuf;
	}

	const char * printHex(const word * a, const int length) {
		char * p = sbuf;
		for (int i = 0; i < length; i++) {
			sprintf(p, "%04X ", a[i]);
			p += 5;
		}
		return sbuf;
	}

	const char * printHex(const byte b) {
		sprintf(sbuf, "%02X", b);
		return sbuf;
	}

	const char * printHex(const word w) {
		sprintf(sbuf, "%04X", w);
		return sbuf;
	}
};

#endif /* MONITOR_H_ */
