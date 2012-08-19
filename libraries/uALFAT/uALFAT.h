/*
 *  uALFAT.h
 *  
 *
 *  Created by ‰º‰’ ^ˆê on 10/08/12.
 *  Copyright 2010 ‹ãBH‹Æ‘åŠwî•ñHŠw•”. All rights reserved.
 *
 */

#ifndef uALFAT_h
#define uALFAT_h

#include <inttypes.h>
#if ARDUINO >= 100
#include "Arduino.h"
#include <SoftwareSerial.h>
#else
#include "WProgram.h"
#include "NewSoftSerial.h"
#endif

//#define RX_BUFFER_SIZE 128
/*
struct ringbuffer {
	char buffer[RX_BUFFER_SIZE];
	int head;
	int tail;
};
*/

class GHIuALFAT : public Print {
private:
	SoftwareSerial serport;
	byte resetPin;
//	char buffer[RX_BUFFER_SIZE];
	
public:
	GHIuALFAT(byte rxpin, byte txpin, byte rstpin = 0xff);
	
	byte start();
	byte reset();
	byte version(char*);
	byte currentTime(char *);
	byte currentDate(char *);
	byte card_init();
	byte file_exists(char *);
	byte file_open(char*, byte, char);
	byte file_close(byte);
	byte file_read(byte, char*, long int);
	byte file_write(byte, char *, long int);
	byte file_print(byte, char *);
	
	byte available(void);
	int read();
#if ARDUINO >= 100
	virtual size_t write(uint8_t);
#else
	virtual void write(uint8_t);
#endif
    using Print::write; // pull in write(str) and write(buf, size) from Print
	
	byte waitResponse(char * p = 0);
	int getResponse(char*);
};

#endif
