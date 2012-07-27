/*
 *  Monitor.h
 *  
 *
 *
 */
#ifndef Monitor_H
#define Monitor_H

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <Wiring.h>
#endif
#include <Stream.h>
#include <HardwareSerial.h>

class Monitor : public Stream {
private:
	void printNumber(byte i, byte b);
	const enum {
		UNDEFINED = 0,
		HARDWARESERIAL,
#ifdef SoftwareSerial_h
		SOFTWARESERIAL,
#endif
	} ser_class;

protected:
	Stream & port;
	int curr_base;

public:
	static const char endl = '\n';
	static const char cr = '\r';
	static unsigned long asBCD(unsigned long lval);
	static int asHexadecimal(char * str, int offset);
	static int asBytes(byte[], int limit = 0);

	Monitor(Stream & st) : port(st), ser_class(UNDEFINED), curr_base(DEC) { }
	Monitor(HardwareSerial & st) : port(st), ser_class(HARDWARESERIAL), curr_base(DEC) { }
#ifdef SoftwareSerial_h
	Monitor(SoftwareSerial & st) : port(st), ser_class(SOFTWARESERIAL), curr_base(DEC) { }
#endif
//	~Monitor() { }

	void begin(long baud) {
		switch(ser_class) {
		case HARDWARESERIAL:
			((HardwareSerial&) port).begin(baud);
			break;
#ifdef SoftwareSerial_h
		case SOFTWARESERIAL:
			((SoftwareSerial&) port).begin(baud);
			break;
#endif
		}
	}
	// for communication speed specification

	Stream & stream() {
		return port;
	}

	// Overrinding Print's inherited virtual functions
#if ARDUINO >= 100
	virtual size_t write(uint8_t b) {
		return port.write(b);
	}
#else
	virtual void write(uint8_t b) {
		port.write(b);
		return;
	}
#endif
	using Print::write;
	using Print::print;
#if ARDUINO >= 100
	size_t print(const char * s, byte base) {
		char * p = (char*) s;
		while (*p != 0) {
			printNumber(*p, base);
			print(' ');
			p++;
		}
		return p -s;
	}
#else
	void print(const char * s, byte base) {
		char * p = (char*) s;
		while (*p != 0) {
			printNumber(*p, base);
			print(' ');
			p++;
		}
	}
#endif

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

	int read(byte rcv[], const int maxlen, const long limit = 10);
	int skipRead(const int len, const long limit = 10) { return read(NULL, len, limit); }
	boolean wait(const byte msg[], int len, long limit);

//	static char * byteString(char * buf, const byte[], const int, const byte = HEX);
	const Monitor & printHexString(const byte[], const int);
	inline const Monitor & printHexString(const int d) {
		return printHexString((byte*) &d, 2);
	}
	inline const Monitor & printHexString(const unsigned int d) {
		return printHexString((byte*) &d, 2);
	}
	inline const Monitor & printHexString(const long d) {
		return printHexString((byte*) &d, 4);
	}
	inline const Monitor & printHexString(const unsigned long d) {
		return printHexString((byte*) &d, 4);
	}


	//byte readLine(byte buf[], int lim, const char delims[]);
	int readToken(char buf[], long timeout = 50);
	int readNumber(char buf[], long timeout = 50);

	const Monitor & setbase(const int b) {
		curr_base = b;
		return *this;
	}
};

template<class T>
inline Stream &operator <<(Stream &stream, T arg) {
	stream.print(arg);
	return stream;
}

inline Stream &operator <<(Stream &stream, const Monitor & m) {
	return stream;
}

#endif
