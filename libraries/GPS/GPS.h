/*
 * GPS.h
 *
 *  Created on: 2012/03/25
 *      Author: sin
 */

#ifndef GPS_H_
#define GPS_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <ctype.h>
//#include <inttype.h>

class GPS {
	Stream & port;
	char buffer[128];
	int pos;

public:
	long time; 			// time in decimal HHMMSS.ss x100 format
	long caldate; 		// calender date in decimal YYYYMMDD format
	long latitude;		// latitude in +(E)/-(W)*** deg MM.mm' x100 format
	long longitude;		// the same as above
	float altitude;
	float grndspeed;
	float trackangle;
	float dilution;

public:
	GPS(Stream & serial) : port(serial) {
		pos = 0;
		buffer[0] = 0;

		time = 0;
		caldate = 0;
		latitude = 0;
		longitude = 0;
		dilution = 0;
	}

	char * sentence() { return buffer; }
	void clear() { buffer[0] = 0; pos = 0; }
	const byte sentenceType();

	boolean update(const byte typemask);
	boolean getSentence();

	void copyItem(char * dst, int inum);
	const char * getPtr(int inum);
	char getChar(int inum);
	long getLong(int inum);
	float getFloat(int inum);

public: // const

enum {
	NONE = 0,
	GGA = 1<<0,
	GGA_UTC = 1, GGA_LATITUDE = 2, GGA_LONGITUDE = 4, GGA_FIXQUALITY = 6,
	GGA_NUMBEROFSATELLITES = 7, GGA_DILUTION = 8, GGA_ALTITUDE = 9,
	GLL = 1<<2,
	GSA = 1<<3,
	GSV = 1<<4,
	RMC = 1<<5,
	RMC_UTC = 1, RMC_LATITUDE = 3, RMC_LONGITUDE = 5, RMC_GROUNDSPEED = 7, RMC_TRACKANGLE = 8, RMC_DATE = 9,
	ZDA = 1<<7,
};

	/*
	 *
AAM - Waypoint Arrival Alarm
ALM - Almanac data
APA - Auto Pilot A sentence
APB - Auto Pilot B sentence
BOD - Bearing Origin to Destination
BWC - Bearing using Great Circle route
DTM - Datum being used.
GGA - Fix information
GLL - Lat/Lon data
GRS - GPS Range Residuals
GSA - Overall Satellite data
GST - GPS Pseudorange Noise Statistics
GSV - Detailed Satellite data
MSK - send control for a beacon receiver
MSS - Beacon receiver status information.
RMA - recommended Loran data
RMB - recommended navigation data for gps
RMC - recommended minimum data for gps
RTE - route message
TRF - Transit Fix Data
STN - Multiple Data ID
VBW - dual Ground / Water Spped
VTG - Vector track an Speed over the Ground
WCV - Waypoint closure velocity (Velocity Made Good)
WPL - Waypoint Location information
XTC - cross track error
XTE - measured cross track error
ZTG - Zulu (UTC) time and time to go (to destination)
ZDA - Date and Time
	 */
};

#endif /* GPS_H_ */
