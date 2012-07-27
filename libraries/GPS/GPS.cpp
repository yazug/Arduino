/*
 * GPS.cpp
 *
 *  Created on: 2012/03/26
 *      Author: sin
 */

#include "GPS.h"

boolean GPS::update(const byte typemask) {
	long lasttime = time;
	if ( ! getSentence() )
		return false;
	switch(sentenceType() & typemask) {
	case GGA:
		time = getFloat(GGA_UTC)*100;
	    latitude = getFloat(GGA_LATITUDE)*100;
	    if ( getChar(GGA_LATITUDE+1) == 'S' )
	    	latitude = -latitude;
	    longitude = getFloat(GGA_LONGITUDE)*100;
	    if ( getChar(GGA_LONGITUDE+1) == 'W' )
	    	longitude = -longitude;
	    dilution = getFloat(GGA_DILUTION);
	    altitude = getFloat(GGA_ALTITUDE);
    	return (time != 0) && (lasttime != time);
		break;
	case RMC:
		time = getFloat(RMC_UTC)*100;
		latitude = getFloat(RMC_LATITUDE)*100;
	    if ( getChar(RMC_LATITUDE+1) == 'S' )
	    	latitude = -latitude;
		longitude = getFloat(RMC_LONGITUDE)*100;
	    if ( getChar(RMC_LONGITUDE+1) == 'W' )
	    	longitude = -longitude;
		grndspeed = getFloat(RMC_GROUNDSPEED);
		trackangle = getFloat(RMC_TRACKANGLE);
		caldate = getLong(RMC_DATE);
		caldate = (2000 + (caldate % 100)) * 10000
				+ (caldate / 100 % 100) * 100 + (caldate / 10000);
    	return (time != 0) && (lasttime != time);
		break;
	case ZDA:
		caldate = getLong(4) * 10000 + getLong(3) * 100 + getLong(2);
		return true;
		break;
	}
	return false;
}

boolean GPS::getSentence() {
	char c;
	while (port.available()) {
		c = (char) port.read();
		if (!isprint(c)) {
			if (pos == 0)
				continue;
			break;
		}
		if (c == '$') {
			pos = 0;
			buffer[0] = 0;
			continue;
		}
		buffer[pos] = c;
		pos++;
		buffer[pos] = 0;
	}
	if (isprint(c))
		return false;
	if (pos == 0)
		return false;
	//
	int i;
	byte xsum = 0;
	for (i = 0; buffer[i] && buffer[i] != '*'; i++) {
		xsum ^= buffer[i];
	}
	if (buffer[i] == '*') {
		i++;
		if (xsum == (byte) strtol(buffer + i, NULL, 16)) {
			pos = 0;
			return true;
		}
	}
	return false;
}


void GPS::copyItem(char * dst, int inum) {
	int i;
	int icount = 0;
	int p = 0; // if buffer[0] == $ then p must be 1.
	dst[0] = 0;
	for (i = 0; buffer[i]; i++) {
		if (buffer[i] == ',' or buffer[i] == '*') {
			if (icount == inum) {
				strncpy(dst, buffer + p, i - p);
				dst[i - p] = 0;
				return;
			}
			if (buffer[i] == '*')
				break;
			icount++;
			p = i + 1;
		}
	}
	return;
}

const char * GPS::getPtr(int inum) {
	int i;
	int icount = 0;
	int p = 0; // if buffer[0] == $ then p must be 1.
	for (i = 0; buffer[i]; i++) {
		if (icount == inum) {
			return buffer+i;
		}
		if ( buffer[i] == ',' ) {
			icount++;
		}
	}
	return NULL;
}

char GPS::getChar(int inum) {
	//char tmp[16] = "";
	//copyItem(tmp, inum);
	return *getPtr(inum); //tmp[0];
}

long GPS::getLong(int inum) {
//	char tmp[16] = "";
//	copyItem(tmp, inum);
	return atol(getPtr(inum)); //tmp);
}

float GPS::getFloat(int inum) {
//	char tmp[16] = "";
//	copyItem(tmp, inum);
	return atof(getPtr(inum)); //tmp);
}

const byte GPS::sentenceType() {
	char * p = buffer + 2;
	char c = *p++;
	if (c == 'G') {
		if (strncmp(p, "GA", 2) == 0)
			return GGA;
		if (strncmp(p, "LL", 2) == 0)
			return GLL;
		if (strncmp(p, "SA", 2) == 0)
			return GSA;
		if (strncmp(p, "SV", 2) == 0)
			return GSV;
	} else if ( c == 'R') {
		if (strncmp(p, "MC", 2) == 0)
			return RMC;
	} else if ( c == 'Z') {
		if (strncmp(p, "DA", 2) == 0)
			return ZDA;
	}
	return 0;
}

