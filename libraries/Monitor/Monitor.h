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
// stdlib.h is included in Arduino.h

const char endl = '\n';

template<class T>
inline Stream &operator <<(Stream &stream, T arg) {
	stream.print(arg);
	return stream;
}

class Monitor : public Stream {
	Stream & stream;
//	char * sbuf;

public:
    virtual size_t write(uint8_t b) { return stream.write(b); }
    using Print::write;

    virtual int available() { return stream.available(); }
    virtual int read() { return stream.read(); }
    virtual int peek() { return stream.peek(); }
    virtual void flush() { stream.flush(); }

	Monitor(Stream & s) : stream(s) {
	}

	using Print::print;
	size_t print(const byte b) {
		print(b>>4, HEX);
		print(b & 0x0f);
		return 2;
	}

	void printHex(byte * a, const int length);
	void printHex(const char * s, const int length);
	void printHex(const word * a, const int length);

	word readToken(char buf[], long timeout = 200);
	boolean readLine(char buf[], long wait = 10);
};

#endif /* MONITOR_H_ */
