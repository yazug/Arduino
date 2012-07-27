/*
 * MindSet.h
 *
 *  Created on: 2012/03/04
 *      Author: sin
 */

#ifndef MINDSET_H_
#define MINDSET_H_

#include <Arduino.h>

class MindSet {

	const static int SYNC = 0xAA;
	const static int EXCODE = 0x55;

	const static byte POOR_QUALITY = 0x02;
	const static byte eSence_ATTENTION = 0x04;
	const static byte eSence_MEDITATION = 0x05;
	const static byte RAW_WAVE_VALUE = 0x80;
	const static byte ASIC_EEG_VALUE = 0x83;

public:
	const static byte PayloadBuffer_size = 174;
	const static byte pos_sync1 = 0;
	const static byte pos_sync2 = 1;
	const static byte pos_paylength = 2;
	const static byte pos_payhead = 3;

	enum EEG_index {
		DELTA = 0,
		THETA,
		LOW_ALPHA,
		HIGH_ALPHA,
		LOW_BETA,
		HIGH_BETA,
		LOW_GAMMA,
		MID_GAMMA,
		EEG_size
	};

private:
	Stream & btPort;

public:
	byte payload[PayloadBuffer_size];

//MindSet readings
	int rawWave; // raw brain wave value
	byte attention; // attention esense value 0, 1 to 100, focus
	byte meditation; // meditation esense value 0, 1 to 100, calmness or relaxed
	byte poorSignal;

// EEG Readings
	long EEG[EEG_size];
	/*
	long delta; // 0.5 - 2.75Hz
	long theta; // 3.5 - 6.75Hz
	long lowAlpha; // 7.5 - 9.25Hz
	long highAlpha; // 10 - 11.75Hz
	long lowBeta; // 13 - 16.75Hz
	long highBeta; // 18 - 29.75Hz
	long lowGamma; // 31 - 39.75Hz
	long midGamma; // 41 - 49.75Hz
*/
	boolean EEG_updated;

public:
	MindSet(Stream & port) :
			btPort(port) {
		//MindSet readings
		rawWave = 0; // raw brain wave value
		attention = 0; // attention esense value
		meditation = 0; // meditation esense value
		poorSignal = 0;
		//
		EEG_updated = false;
	}

	boolean getPacket();
	void parsePayload();
	byte packetLength() {
		return payload[pos_paylength] + 4;
	}
	byte payloadLength() {
		return payload[pos_paylength];
	}
	byte packetChecksum() {
		return payload[pos_payhead + payloadLength()];
	}

	byte poorSignalQuality() {
		return poorSignal;
	}
	boolean notReliable() {
		return (attention == 0) || (meditation == 0);
	}
};

#endif /* MINDSET_H_ */
