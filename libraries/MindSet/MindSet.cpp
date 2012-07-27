/*
 * MindSet.cpp
 *
 *  Created on: 2012/03/04
 *      Author: sin
 */

#include "MindSet.h"

boolean MindSet::getPacket() {
	byte xsum = 0;
	payload[2] = 0;
	int pos = 0;
	while (btPort.available() > 0) {
//		Serial.print(", pos=");
//		Serial.print(pos);
		payload[pos] = btPort.read();
		switch (pos) {
		case pos_sync1:
			if (payload[pos_sync1] == SYNC)
				pos++;
				break;
		case pos_sync2:
			if (payload[pos_sync2] == SYNC)
				pos++;
			else
				pos = 0;
				break;
		case pos_paylength:
			if (payload[pos_paylength] == 170 ) {
				break;
			} else if (payload[pos_paylength] < 170) {
				pos++;
				break;
			} else {
				payload[pos_paylength] = 0;
				pos = 0;
				break;
			}
		}
		if ( pos == 3)
			break;
		while ( !(btPort.available() > 0) );
	}
//	Serial.println("hi.");
	if (payload[pos_paylength] > 0) {
		// header three bytes check has been done.
		for (byte i = 0; i < payload[pos_paylength];) {
			if (!(btPort.available() > 0))
				continue;
			payload[i + pos_payhead] = btPort.read();
			xsum += payload[i + pos_payhead];
			i++;
		}
		payload[payload[pos_paylength] + pos_payhead] = btPort.read();
		if ((xsum + packetChecksum()) & 0xff != 255) {
			/*
			 Serial.println();
			 Serial.print(" xsum = ");
			 Serial.print(xsum);
			 Serial.print(",checksum = ");
			 Serial.print(packetChecksum());
			 Serial.println();
			 */
			return false;
		}
		return true;
	}
	return false;
}


void MindSet::parsePayload() {
	int extCodeLevel;
	int vlength;
	byte code;
	long t;
	int e;
	int i = pos_payhead;
	EEG_updated = false;
	while (i < packetLength()) {
		//get Extended Code Level
		extCodeLevel = 0;
		while (payload[i] == EXCODE && i < packetLength()) {
			extCodeLevel++;
			i++;
		}
		code = payload[i];
		i++;
		if (code < 0x80 && extCodeLevel == 0) {
			// single value byte
			switch (code) {
			case POOR_QUALITY:
				poorSignal = payload[i];
				break;
			case eSence_ATTENTION:
				attention = payload[i]; // <8>
				break;
			case eSence_MEDITATION:
				meditation = payload[i]; // <8>
				break;
			}
			i++;
		} else {
			vlength = payload[i++];
			switch (code) {
			case RAW_WAVE_VALUE:
				rawWave = ((int) payload[i]) << 8 | payload[i + 1];
				break;

			case ASIC_EEG_VALUE: // EEG waves
				for (e = 0; e < EEG_size; e++ ) {
					t = payload[i+(e*3)];
					t <<= 8;
					t |= payload[i+(e*3)+1] | t;
					t <<= 8;
					t |= payload[i+(e*3)+2];
					EEG[e] = t; //((long)payload[i+(e*3)])<<16 | (payload[i+(e*3)+1])<<8 | payload[i+(e*3)+2];
				}
				EEG_updated = true;
				break;
			case 0xD0:
			case 0xD1: // Headset Not Found
			case 0xD2: // Headset Disconnected
			case 0xD3: // Request Denied
				//		case -70:
				break;
			case 0x55:
			case 0xaa:
				break;
			}
			i += vlength;
		} // for
	}
}

