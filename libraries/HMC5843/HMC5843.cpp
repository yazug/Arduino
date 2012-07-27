#include <Wire.h>
#include "HMC5843.h"

/* PUBLIC METHODS */

HMC5843::HMC5843() {
}

// note that you need to wait at least 5ms after power on to initialize
void HMC5843::begin(const uint8_t gain, const uint8_t mode) {
	Wire.beginTransmission(HMC5843_ID);
	Wire.send(CRB);
	Wire.send(gain);
	//	Wire.endTransmission();
	//	Wire.beginTransmission(HMC5843_ID);
	//	Wire.send(MR);
	Wire.send(mode);
	Wire.endTransmission();	
}

void HMC5843::mode(const uint8_t mode) {
	Wire.beginTransmission(HMC5843_ID);
	Wire.send(MR);
	Wire.send(mode);
	Wire.endTransmission();	
}

void HMC5843::gain(const uint8_t gain) {
	Wire.beginTransmission(HMC5843_ID);
	Wire.send(CRB);
	Wire.send(gain);
	Wire.endTransmission();	
}

int HMC5843::countsPerMilliGauss(const uint8_t gain) {
	switch (gain) {
		case Gain1620:
			return 1620;
		case Gain1300:
			return 1300;
		case Gain970:
			return 970;
		case Gain780:
			return 780;
		case Gain530:
			return 530;
		case Gain460:
			return 460;
		case Gain390:
			return 390;
	}
	return 280;
}

void HMC5843::measured(int & x, int & y, int & z) {
	unsigned char xh, xl, yh, yl, zh, zl;
	
	Wire.beginTransmission(HMC5843_ID);
	Wire.send(DOXMR);
	Wire.endTransmission();	

	Wire.requestFrom(HMC5843_ID, (uint8_t)7);
	xh = Wire.receive();
	xl = Wire.receive();
	yh = Wire.receive();
	yl = Wire.receive();
	zh = Wire.receive();
	zl = Wire.receive();
//	Wire.endTransmission();
	x = xh<<8 | xl;
	y = yh<<8 | yl;
	z = zh<<8 | zl;
}
