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

class Monitor : public Stream {
	Stream & stream;
//	char * sbuf;

public:
    virtual size_t write(uint8_t b) { return stream.write(b); }
    using Print::write;
    using Print::print;

    virtual int available() { return stream.available(); }
    virtual int read() { return stream.read(); }
    virtual int peek() { return stream.peek(); }
    virtual void flush() { stream.flush(); }

	Monitor(Stream & s) : stream(s) {
	}



	void printHex(byte * a, const int length) {
		for (int i = 0; i < length; i++) {
			print(*a>>4, HEX);
			print(*a&0x0f, HEX);
			print(' ');
			a++;
		}
		return;
	}

	void printHex(const char * s, const int length) {
		for (int i = 0; i < length; i++) {
			if (isprint(s[i]))
				print(s[i]);
			else {
				print("$");
				print((byte)s[i], HEX);
			}
		}
		return;
	}

	void printHex(const word * a, const int length) {
		for (int i = 0; i < length; i++) {
			print(a[i]>>12, HEX);
			print(a[i]>>8&0x0f, HEX);
			print(a[i]>>4&0x0f, HEX);
			print(a[i]&0x0f, HEX);
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
};

#endif /* MONITOR_H_ */
