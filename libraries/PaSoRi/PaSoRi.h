#ifndef _PaSoRi_H_
#define _PaSoRi_H_

#include "WProgram.h"
#include <Spi.h>
#include <Max3421e.h>
#include <Usb.h>

//#define DEBUG

#define PASORI_VID_HI 0x05
#define PASORI_VID_LO 0x4c

#define PASORI_PID_S320_HI 0x01
#define PASORI_PID_S320_LO 0xbb
#define PASORI_PID_S330_HI 0x02
#define PASORI_PID_S330_LO 0xe1

#define POLLING_ANY    0xffff
#define POLLING_SUICA  0x0003
#define POLLING_EDY    0xfe00

#define FELICA_CMD_POLLING 0
#define FELICA_ANS_POLLING 1
#define FELICA_CMD_REQUEST_RESPONSE 4
#define FELICA_ANS_REQUEST_RESPONSE 5
#define FELICA_CMD_READ_WITHOUT_ENCRYPTION 6
#define FELICA_ANS_READ_WITHOUT_ENCRYPTION 7
#define FELICA_CMD_WRITE_WITHOUT_ENCRYPTION 8
#define FELICA_ANS_WRITE_WITHOUT_ENCRYPTION 9
#define FELICA_CMD_PUSH 0xb0
#define FELICA_ANS_PUSH 0xb1

#define PASORI2_CMD_SEND_PACKET 0x5c
#define PASORI2_ANS_SEND_PACKET 0x5d

#define PASORI3_CMD_SEND_PACKET 0xd4
#define PASORI3_ANS_SEND_PACKET 0xd5

#define PASORI3_CMD_SEND_PACKET2 0x42
#define PASORI3_CMD_LIST_PASSIVE 0x4a


class PaSoRi {
  MAX3421E Max;
  USB Usb;
  enum { PASORI_S320, PASORI_S330 } mode;
  EP_RECORD ep_record[5];  //endpoint record structure for PaSoRi

  byte idm[8];
  byte pmm[8];

  byte init();

public:
  byte begin();
  void task();

  byte poll(unsigned short syscode = POLLING_ANY, byte rfu = 0, byte timeslot = 0);
  const byte* getIDm() { return idm; }
  const byte* getPMm() { return pmm; }
  int read_without_encryption02(int servicecode, byte addr, byte b[16]);
  byte mobile_felica_push_url(int len, const char *url);

  // low level transmission
  byte send_packet(int len, const byte *data);
  byte send(int len, byte *buf);
  byte recv(int len, byte *buf);
};

#endif
