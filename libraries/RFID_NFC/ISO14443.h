/*
 * NFCCard.h
 *
 *  Created on: 2012/07/24
 *      Author: sin
 */

#ifndef ISO14443Card_H_
#define ISO14443Card_H_

//#include "PN532_I2C.h"
static const byte TypeA = 0x00;
static const byte TypeF_212kb = 0x01;
static const byte TypeF_424kb = 0x02;
static const byte TypeB = 0x03;
static const byte Type_Mifare = 0x10;
static const byte Type_FeliCa212kb = 0x11;
static const byte Type_FeliCa424kb = 0x12;


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


struct TFCF {
    byte res0[2];
    char id[8];
    byte res1[4];
    char issue[1];
    byte res2;
    char kana[16];
    byte res3[16];
    char goodthru[8];
    byte res4[8];
};

struct TMagTape {
  char idtype[2];
  char id[8];
  char issue[1];
  char res0[5];
  word kanji[8];
  char dayofbirth[7];
  char gender[1];
  char dayofissue[7];
  char res1[1];
};

struct ISO14443Card {
  static const byte data_size = 18;
  //
  byte type;
  union {
    struct {
      byte IDLength;
      union {
      byte UID[7];
      byte NUID[7];
      };
    };
    struct {
      byte IDm[8];
      byte PMm[8];
      byte SysCode[2];
    };
    byte rawIDData[data_size];
  };

  ISO14443Card() {
    init();
  }

  ISO14443Card(const byte * raw) {
    set(raw);
  }
  
  ISO14443Card(const byte & card) {
    set(card);
  }

  void set(const ISO14443Card & card) {
    type = card.type;
    switch(type) {
    case Type_FeliCa212kb: // Felica
      memcpy(rawIDData, card.rawIDData, 18);
      break;
    default: // Mifare
      memcpy(rawIDData, card.rawIDData, 8);
      break;
    }
  }

  ISO14443Card & operator=(const ISO14443Card & c) {
    set(c);
    return *this;
  }

  void set(const byte tid, const byte * raw) {
    //PN532::printHexString(raw, 16);
    type = tid;
    byte len;
    switch (type) {
      case Type_FeliCa212kb:
      case Type_FeliCa424kb:
      len = raw[1];
      memcpy(IDm, raw + 3, 8);
      memcpy(PMm, raw + 11, 8);
      if ( len == 20)
        memcpy(SysCode, raw + 19, 2);
      break;
      case Type_Mifare:
    default: // Mifare 106k TypeA
      IDLength = raw[4];
      memcpy(UID, raw + 5, IDLength);
      break;
    }
  }

  void init() {
    type = 0xff;
    memset(rawIDData, 0, 8);
  }

  const boolean operator==(const ISO14443Card & c) const {
    if ( type == c.type ) {
      return memcmp(rawIDData, c.rawIDData, 8) == 0;
    }
    return false;
  }
  
  inline const boolean operator!=(const ISO14443Card & c) const {
    return !(operator==(c));
  }
};


#endif /* NFCCARD_H_ */



