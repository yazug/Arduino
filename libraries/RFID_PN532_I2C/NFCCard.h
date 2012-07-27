/*
 * NFCCard.h
 *
 *  Created on: 2012/07/24
 *      Author: sin
 */

#ifndef NFCCARD_H_
#define NFCCARD_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

struct NFCCard {
	static const byte data_size = 18;
	byte type;
	byte NFCIDLength;
	union {
		struct {
			byte NFCID1[7];
		} mifare;
		struct {
			byte NFCID2[8];
			byte PAD[8];
			byte SYST_CODE[2];
		} felica;
		byte data[data_size];
	};

	NFCCard() {
		type = PN532::Type_GenericPassiveTypeA;
		NFCIDLength = 0;
	}

	/*
	 NFCCard(const NFCCard & c) {
	 type = c.type;
	 NFCIDLength = c.NFCIDLength;
	 memcpy(data, c.data, 8);
	 }
	 */

	NFCCard(const byte * raw, const byte tp, const byte length = 8) {
		type = tp;
		NFCIDLength = length;
		memcpy(data, raw, NFCIDLength);
	}

	/*
	 void set(const byte * raw, const byte idlen, const byte cardtype) {
	 type = cardtype;
	 NFCIDLength = min(idlen, 8);
	 memcpy(NFCID1, raw, NFCIDLength);
	 }
	 */

	void set(const NFCCard & c) {
		type = c.type;
		NFCIDLength = c.NFCIDLength;
		memcpy(data, c.data, NFCIDLength);
	}

	NFCCard & operator=(const NFCCard & c) {
		set(c);
		return *this;
	}

	void setInListPassiveTarget(const byte ct,
			const byte len, const byte * raw) {
		//PN532::printHexString(raw, 16);
		type = ct; //raw[offset+1] + (raw[offset+2]<<8);
		switch (type) {
		case PN532::Type_FeliCa212kb:
		case PN532::Type_FeliCa424kb:
			memcpy(felica.NFCID2, raw + 3, 8);
			memcpy(felica.PAD, raw + 11, 8);
			if ( len == 19 + 2)
				memcpy(felica.SYST_CODE, raw + 19, 2);
			break;
		case PN532::Type_Mifare:
		default:
			NFCIDLength = raw[4];
			memcpy(mifare.NFCID1, raw + 5, NFCIDLength);
			break;
		}
	}

	const byte * NFCID1() {
		return mifare.NFCID1;
	}

	inline const byte IDLength() {
		return NFCIDLength;
	}

	inline const byte * UID() {
		return felica.NFCID2;
	}

	inline const byte * IDm() {
		return felica.NFCID2;
	}

	inline const byte * PMm() {
		return felica.PAD;
	}

	inline const byte * SystemCode() {
		return felica.SYST_CODE;
	}

	void clear() {
		type = 0;
		NFCIDLength = 0;
	}

	const boolean operator==(const NFCCard & c) const {
		if ( type == c.type ) {
			return memcmp(data, c.data, NFCIDLength) == 0;
		}
		return false;
	}
};


#endif /* NFCCARD_H_ */
