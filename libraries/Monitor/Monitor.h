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
#include <HardwareSerial.h>

#include <string.h>

const char endl = '\n';
const char cr = '\r';

template<class T>
inline Stream &operator <<(Stream &stream, T arg) {
	stream.print(arg);
	return stream;
}

class Monitor: public Stream {
	HardwareSerial & port;
	byte curr_radix;
	char * sbuf;
public:
	Monitor(HardwareSerial & hs, char * buf) : port(hs), curr_radix(HEX), sbuf(buf) {}

	virtual size_t write(uint8_t b) {
		return port.write(b);
	}
	using Print::write;
	using Print::print;

	// Over-riding Stream's virtual functions
	virtual int available() {
		return port.available();
	} // may be overridden
	virtual int read() {
		return port.read();
	}
	virtual int peek() {
		return port.peek();
	}
	virtual void flush() {
		return port.flush();
	}
	//

	Monitor & radix(byte b) {
		sprintf(sbuf,"%02X", b);
		port.print(sbuf);
		return *this;
	}
	Monitor & radix(word w) {
		sprintf(sbuf,"%04X", w);
		port.print(sbuf);
		return *this;
	}
	Monitor & radix(unsigned long l) {
		sprintf(sbuf,"%08X", l);
		port.print(sbuf);
		return *this;
	}

};


#endif /* MONITOR_H_ */
