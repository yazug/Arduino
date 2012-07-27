/*
 *  uALFAT.cpp
 *  
 *
 *  Created by â∫âí ê^àÍ on 10/08/12.
 *  Copyright 2010 ã„èBçHã∆ëÂäwèÓïÒçHäwïî. All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
//#include "wiring.h"
//#include "wiring_private.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "uALFAT.h"

GHIuALFAT::GHIuALFAT(byte rx, byte tx, byte rst) : serport(rx, tx) {
	resetPin = rst;
	if (resetPin != 0xff) {
		pinMode(resetPin, OUTPUT);
	}
//	buffer[0] = 0;
}

byte GHIuALFAT::start() {
	serport.begin(9600);
	return reset();
}

byte GHIuALFAT::reset() {
	if ( resetPin == 0xff ) 
		return 0x00;
	digitalWrite(resetPin, LOW);
	delay(33);
	digitalWrite(resetPin, HIGH);
	
	waitResponse(); // only CR
	waitResponse(); // GHI Electronics, LLC
	waitResponse(); // ----------------------
	waitResponse(); // Boot Loader <version>
	waitResponse(); // uALFAT(TM) <version>
	return waitResponse(); // expect !00<cr>
}

byte GHIuALFAT::version(char * resp) {
	print("V\r");
	waitResponse(resp);
	return waitResponse();
}

byte GHIuALFAT::card_init() {
	print("I\r");
	return waitResponse();
}

byte GHIuALFAT::file_exists(char * fname_and_resp) {
	byte ack;
	print("? ");
	print(fname_and_resp);
	print("\r");
	ack = waitResponse();
	if ( ack != 0) {
		*fname_and_resp = 0;
		return ack;
	}
	waitResponse(fname_and_resp);
	return waitResponse();	
}

byte GHIuALFAT::file_open(char* fname, byte fhNo, char mode) {
	if (fhNo > 3)
		return -1;
	switch (mode) {
		case 'R':
		case 'W':
		case 'A':
			break;
		default:
			mode = 'R';
	}
	print("O ");
	print(fhNo, DEC);
	print((char)mode);
	print((char)'>');
	print(fname);
	print((char)'\r');
	return waitResponse();	
}

byte GHIuALFAT::file_close(byte fhNo) {
	print("C ");
	print(fhNo, DEC);
	print((char)'\r');
	return waitResponse();	
}

byte GHIuALFAT::file_read(byte fhNo, char * contents, long upb) {
	byte ack;
	char * lastDollar;
	print("R ");
	print(fhNo, DEC);
	print((char)'\0');
	print((char)'>');
	print(upb, HEX);
	print((char)'\r');
	ack = waitResponse();
	if (ack)
		return ack;
	waitResponse(contents);
	ack = waitResponse((char*)0);
	for (lastDollar = contents; (upb > 0) && (*contents != '\0') ; upb--) {
		if ( *contents == '$')
			lastDollar = contents;
	}
	if ( upb > 0)
		*lastDollar = '\0';
	return ack;
}

byte GHIuALFAT::file_write(byte fhNo, char * contents, long upb) {
	byte ack;
	char c;
	print("W ");
	print(fhNo, DEC);
	print((char)'>');
	print(upb, HEX);
	print((char)'\r');
	ack = waitResponse();
	if (ack)
		return ack;
	while ( upb > 0 ) {
		c = *contents;
		print(c);
		if ( c != '\0' )
			contents++;
		upb--;
	}
	waitResponse();
	return waitResponse();
}


byte GHIuALFAT::file_print(byte fhNo, char * contents) {
	return file_write(fhNo, contents, strlen(contents));
}

byte GHIuALFAT::currentTime(char * result) {
	char tmp[24];
	char * p;
	byte resp;
	
	print("G F");
	print((char)'\r');
	waitResponse();	
	waitResponse(result);	
	resp = waitResponse();
	if (resp)
		return resp;
	p = result;
	p += 13;
	strcpy(result, p);
	return resp;
}

byte GHIuALFAT::currentDate(char * result) {
	char tmp[24];
	int dexs[] = { 6, 7, 8, 9, 0, 1, 3, 4 };
	int i;
	byte resp;
	
	print("G F");
	print((char)'\r');
	waitResponse();	
	waitResponse(tmp);	
	resp = waitResponse();
	if (resp)
		return resp;
	for ( i = 0; i < sizeof(dexs); i++) {
		result[i] = tmp[dexs[i]];
	}
	result[8] = 0;
	return resp;
}


int GHIuALFAT::read() {
	return serport.read();
}

void GHIuALFAT::write(byte c) {
	serport.print(c);
}

byte GHIuALFAT::available(void) {
	return serport.available();
}


byte GHIuALFAT::waitResponse(char * resp) {
	char ch;
	boolean ack = false;
	byte acode = 0;
	while ( !available() );
	while ( (ch = read()) != 0x0d ) {
		if ( ack ) {
			acode <<= 4;
			acode |= (ch - '0');
		}
		if (ch == '!')
			ack = true;
		if ( resp != 0 ) 
			*resp++ = ch;
		while (!available());
	}
	if ( resp != 0 ) 
		*resp= '\0';
	if ( ack )
		return acode;
	else
		return -1;
}

int GHIuALFAT::getResponse(char * buffer) {
	int count = 0;
	char c;
	int i;
	
	if (available()) {
		while ( (c = read()) != 0x0d ) {
			*buffer++ = c ;
			while (!available());
		}
		*buffer= '\0';
	}
	return count;
}

