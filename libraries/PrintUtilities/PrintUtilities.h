/*
 * PrintUtilities.h
 *
 *  Created on: 2012/04/02
 *      Author: sin
 */

#ifndef PRINTUTILITIES_H_
#define PRINTUTILITIES_H_

#include <Arduino.h>

namespace prt {

static const char endl = '\n';
static const char cr = '\r';

char * printOn(long val, char * format) {
	char * buf = format;
	int len = 0;
	int digits = 0;
	char s;
	if ( val < 0 ) {
		val = -val;
		s = '-';
	} else {
		s = '+';
	}
	while (buf[len]) {
		len++;
		if (isdigit(buf[len]))
			digits++;
	}
	len--;
	for (; len >= 0; len--) {
		if (isdigit(buf[len])) {
			digits--;
			buf[len] = '0' + (val % 10);
			val /= 10;
		} else if (buf[len] == '+' or buf[len] == '-') {
			//Serial.print(" len = "); Serial.print(len);
			buf[len] = s;
			break;
		}
	}
	return format;
}

char * printOn(double fval, char * buf) {
	int len = 0, fracpos = 0;
	char sgn;
	if (fval >= 0)
		sgn = '+';
	else {
		sgn = '-';
		fval = -fval;
	}
	while (buf[fracpos] && buf[fracpos] != '.') {
		fracpos++;
	}
	len = fracpos - 1;
	Serial.print("len = ");
	Serial.println(len);
	Serial.print("fracpos = ");
	Serial.println(fracpos);
	long val = (long) fval;
	fval -= val;
	if (buf[fracpos] == '.') {
		fracpos++;
		for (; buf[fracpos] != 0; fracpos++) {
			fval = fval * 10;
			buf[fracpos] = '0' + ((int) fval % 10);
		}
	}
	for (; len >= 0; len--) {
		buf[len] = '0' + (val % 10);
		val /= 10;
	}
	return buf;
}

// Generic template
template<class T>
inline Stream &operator <<(Stream &stream, T arg) {
	stream.print(arg);
	return stream;
}

}

#endif /* PRINTUTILITIES_H_ */
