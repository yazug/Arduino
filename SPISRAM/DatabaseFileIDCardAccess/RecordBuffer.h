#include "TextStream.h"

class RecordBuffer {
  static const unsigned int SizeOfBuffer = 8;
public:
  union {
    struct {
      byte type;
      char DIVISION;
      char PID[8];
      char ISSUE;
      byte NFCID[8];
      uint32_t goodthru;
      byte privileges;
    };
    byte rawbytes[24];
  };

  Stream & printOn(Stream & os) {
    TextStream tos(os);
    switch(type) {
    case 0x11:
      tos.print("FeliCa ");
      break;
    case 0x10:
    default:
      tos.print("Mifare ");
      break;
    }
    tos << DIVISION << "-";
    tos.printBytes(PID, 8);
    tos << "-" << ISSUE << ", ";
    if ( type == 0x10 ) {
      tos.printBytes(NFCID+1, NFCID[0], '.');
    } 
    else {
      tos.printBytes(NFCID, 8, '.');
    }
    tos << " ";
    tos.print(goodthru, HEX);
    tos << " ";
    tos.print(privileges, BIN);
    tos << endl;
    return os;
  }

  RecordBuffer() {
  }

  void init() {
    type = 0;
    NFCID[0] = 0;
    privileges = 0x80;
    goodthru = 0;
  }

  static const int MIFARE = 0x10;
  static const int FCF = 0x11;
  void set(byte tp, char div, char pid[], char no, byte idlen, byte id[]) {
    type = tp;
    DIVISION = div;
    strncpy(PID, pid, 8);
    ISSUE = no;
    NFCID[0] = idlen;
    memcpy(NFCID+1, id, idlen);
  }

};





