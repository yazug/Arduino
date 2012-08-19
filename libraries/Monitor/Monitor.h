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

	void printHex(const byte b) {
		write('0'+(b>>4));
		write('0'+(b&0x0f));
	}

	void printHex(const byte * a, const int length, char gap = ' ');
	void printHex(const char * s, const int length, char gap = 0x00);
	void printHex(const word * a, const int length, char gap = ' ');

	word readToken(char buf[], long timeout = 200);
	boolean readLine(char buf[], int maxlen, long wait = 10);

	static int ithToken(const char buf[], const int item, int & fromindex);
};

#endif /* MONITOR_H_ */
