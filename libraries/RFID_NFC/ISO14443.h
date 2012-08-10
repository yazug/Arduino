/*
 * NFCCard.h
 *
 *  Created on: 2012/07/24
 *      Author: sin
 */

#ifndef ISO14443_H_
#define ISO14443_H_

//#include "PN532_I2C.h"
static const byte TypeA = 0x00;
static const byte TypeF = 0x01;
static const byte TypeF_212kb = 0x01;
static const byte TypeF_424kb = 0x02;
static const byte TypeB = 0x03;
static const byte TypeJewel = 0x03;
static const byte Mifare = 0x10;
static const byte FeliCa212kb = 0x11;
static const byte FeliCa424kb = 0x12;

static const word FELICA_SYSCODE_EDY = 0x00FE;
static const word FELICA_SYSCODE_FCF = 0x00FE;
static const word FELICA_SYSCODE_SUICA = 0x0003;
static const word FELICA_SERVICE_SUICA = 0x090F;
static const word FELICA_SERVICE_EDY = 0x170F;
static const word FELICA_SERVICE_FCF = 0x1a8b;

/*
 char kana[][4] = {
 "xx", "xx", "xx", "xx", "xx", "xx", "wo", "xa",
 "xi", "xu", "xe", "xo", "ya", "yu", "yo", "xtu",
 "-", "A", "I", "U", "E", "O", "KA", "KI",
 "KU", "KE", "KO", "SA", "SHI", "SU", "SE", "SO",
 "aaa", "bbb", "ccc", "ddd", "eee", "NA", "NI", "NU",
 "NE", "NO", "HA", "HI", "FU", "HE", "HO", "MA",
 "MI", "MU", "ME", "MO", "YA", "YU", "YO", "RA",
 "RI", "RU", "RE", "RO", "WA", "NN", "\"", "o",
 };
 */

struct FCF {
	byte res0[2];
	char id[8];
	byte res1[4];
	char issue;
	byte res2;
	char kana[16];
	byte res3[16];
	char goodthru[8];
	byte res4[8];
};

struct IizukaMagTape {
	char idtype[2];
	char id[8];
	char issue;
	char res0[5];
	word kanji[8];
	char dayofbirth[7];
	char gender;
	char dayofissue[7];
	char res1[1];
};

struct ISO14443 {
	static const byte IDdata_size = 18;
	//
	byte type;
	byte IDLength;
	union {
		byte UID[7];
		byte NUID[7];
		struct {
			byte IDm[8];
			byte PMm[8];
			byte SysCode[2];
		};
		byte id[IDdata_size];
	};

	ISO14443() {
		init();
	}

	ISO14443(const byte * raw) {
		set(raw);
	}

	ISO14443(const byte & card) {
		set(card);
	}

	void set(const ISO14443 & card) {
		type = card.type;
		IDLength = card.IDLength;
		switch (type) {
		case FeliCa212kb: // Felica
			memcpy(id, card.id, IDdata_size);
			break;
		default: // Mifare
			memcpy(id, card.id, IDdata_size);
			break;
		}
	}

	ISO14443 & operator=(const ISO14443 & c) {
		set(c);
		return *this;
	}

	void set(const byte tid, const byte * raw) {
		//PN532::printHexString(raw, 16);
		type = tid;
		byte len;
		switch (type) {
		case FeliCa212kb:
		case FeliCa424kb:
			IDLength = 8;
			len = raw[1];
			memcpy(IDm, raw + 3, 8);
			memcpy(PMm, raw + 11, 8);
			if (len == 20)
				memcpy(SysCode, raw + 19, 2);
			break;
		case Mifare:
		default: // Mifare 106k TypeA
			IDLength = raw[4];
			memcpy(UID, raw + 5, IDLength);
			break;
		}
	}

	const char * typeString(char * buf, const byte ttype) {
		switch (ttype) {
		case Mifare:
			strcpy(buf, "Mifare");
			break;
		case FeliCa212kb:
			strcpy(buf, "FeliCa212kb");
			break;
		case FeliCa424kb:
			strcpy(buf, "FeliCa424kb");
			break;
		}
		return buf;
	}

	void init() {
		type = 0xff;
		IDLength = 0;
		memset(id, 0, 8);
	}

	const boolean operator==(const ISO14443 & c) const {
		if ( type == c.type && IDLength == c.IDLength ) {
			return memcmp(id, c.id, 8) == 0;
		}
		return false;
	}

	inline const boolean operator!=(const ISO14443 & c) const {
		return !(operator==(c));
	}
};

#endif /* NFCCARD_H_ */

