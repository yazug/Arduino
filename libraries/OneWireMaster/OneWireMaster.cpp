#include <SoftwareSerial.h>
#include "OneWireMaster.h"

extern "C" {
#include "WConstants.h"
#include <avr/io.h>
#include "pins_arduino.h"
}



OneWireMaster::OneWireMaster(uint8_t rx, uint8_t tx) : port(rx, tx) {
	lastjunct = 0xff;
	// leave disbits[8] as will be
	pinMode(rx, INPUT);
	pinMode(tx, OUTPUT);
	port.begin(9600);
	delayMicroseconds(250);
	write(COMMAND_RESET);
	delayMicroseconds(250);
}

byte OneWireMaster::reset() {
	byte inbyte;
	commandMode();
	write(COMMAND_RESET);
	delayMicroseconds(500);
	inbyte = read();
	return inbyte;// & 0b00000011;
}

byte OneWireMaster::configure(byte param, byte value) {
	commandMode();
	write( (0b01110000 & param<<4) | (0b00001110 & value<<1) | 0b00000001 );
}

void OneWireMaster::write(byte *p, byte length) {
	for ( ; length > 0; length--, p++) {
		write(*p);
		delayMicroseconds(50); //*p = read();
	}
}

word OneWireMaster::diffuse(byte r, byte d) {
	word w16;
	w16 = r & 0x80;
	w16 <<= 1;
	w16 |= r & 0x40;
	w16 <<= 1;
	w16 |= r & 0x20;
	w16 <<= 1;
	w16 |= r & 0x10;
	w16 <<= 1;
	w16 |= r & 0x08;
	w16 <<= 1;
	w16 |= r & 0x04;
	w16 <<= 1;
	w16 |= r & 0x02;
	w16 <<= 1;
	w16 |= r & 0x01;
	w16 <<= 1;
	return w16;
};

word OneWireMaster::collate(byte b0, byte b1) {
	byte d, r;
	r = b1 & 0x80;
	b1 <<= 1;
	d = b1 & 0x80;
	
	r |= b1 & 0x40;
	b1 <<= 1;
	d |= b1 & 0x40;
	r |= b1 & 0x20;
	b1 <<= 1;
	d |= b1 & 0x20;
	r |= b1 & 0x10;
	b1 <<= 1;
	d |= b1 & 0x10;
	
	d |= b0 & 0x01;
	b0 >>= 1;
	r |= b0 & 0x01;
	d |= b0 & 0x02;
	b0 >>= 1;
	r |= b0 & 0x02;
	d |= b0 & 0x04;
	b0 >>= 1;
	r |= b0 & 0x04;
	d |= b0 & 0x08;
	b0 >>= 1;
	r |= b0 & 0x08;
	return ((word)r)<<8 | d;
};


void OneWireMaster::initializeSearchPath() {
	byte i;
	for (i = 0; i < 8; i++) 
		disbits[i] = 0;
	lastjunct = 0xff;	
}

byte OneWireMaster::search(byte path[8]) {
	byte t, i, jc;
	word w;

	t = reset();
	if ( (t & 0b00000011) == 0b11 ) {
		initializeSearchPath();
		return lastjunct;
	}
	// initialize search if path consists of only zeros
	t = 0;
	for (i = 0; i < 8; i++) {
		t |= path[i];
	}
	if ( t == 0 ) {
		initializeSearchPath();
	} else if ( lastjunct < 64 ) {
		t = 1<<(lastjunct%8);
		path[lastjunct/8] |= t;
	}	
	
	dataMode();
	write(SEARCH_ROM);
	t = read(); // should be return "no presense"
	commandMode();
	write(SEARCH_ACCELERATOR_ON);
	dataMode();
	for (i = 0; i < 8; i++) {
		t = path[i];
		w = diffuse(t, 0);
		write(lowByte(w));
		t = read();
		write(highByte(w));
		w = read();
		w = collate(t, lowByte(w));
		path[i] = highByte(w);
		disbits[i] = lowByte(w);
	}
	commandMode();
	write(SEARCH_ACCELERATOR_OFF);

	lastjunct = 0xff;
	for (jc = 0; jc < 64; jc++) {
		t = 1<<(jc%8);
		if ( (disbits[jc/8] & t) != 0 
			&& (path[jc/8] & t) == 0 ) {
			lastjunct = jc;
		}
	}

	return lastjunct;
}

void OneWireMaster::select(byte addr[8]) {
	byte i;
    write(MATCH_ROM);         // Choose ROM
	delayMicroseconds(100);
    write(addr, 8);
}


byte OneWireMaster::crc8( byte *addr, byte len) {
    byte i, j;
    byte crc = 0;
    
    for (i = 0; i < len; i++) {
        uint8_t inbyte = addr[i];
        for (j = 0; j < 8; j++) {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) crc ^= 0x8C;
            inbyte >>= 1;
        }
    }
    return crc;
}
