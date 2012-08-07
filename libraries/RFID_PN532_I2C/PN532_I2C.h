/*
 * PN532_I2C.h
 *
 *  Created on: 2012/06/30
 *      Author: sin
 */

#ifndef PN532_I2C_H_
#define PN532_I2C_H_

//#include <string.h>

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

class PN532 {

	static const byte I2C_READBIT = (0x01);
	static const byte I2C_BUSY = (0x00);
	static const byte I2C_READY = (0x01);
	static const byte I2C_READYTIMEOUT = (20);

	static const byte PREAMBLE = (0x00);
	static const byte STARTCODE_1 = (0x00);
	static const byte STARTCODE_2 = (0xFF);
	static const byte POSTAMBLE = (0x00);

	static const byte HOSTTOPN532 = (0xD4);

	// PN532 Commands
	static const byte COMMAND_GetFirmwareVersion = (0x02);
	static const byte COMMAND_SAMConfiguration = (0x14);

	static const byte COMMAND_InListPassivTarget = (0x4A);
	static const byte COMMAND_InDataExchange = (0x40);
	static const byte COMMAND_InAutoPoll = (0x60);
	static const byte COMMAND_InCommunicateThru = (0x42);

#define PN532_COMMAND_DIAGNOSE              (0x00)
#define PN532_COMMAND_GETFIRMWAREVERSION    (0x02)
#define PN532_COMMAND_GETGENERALSTATUS      (0x04)
#define PN532_COMMAND_READREGISTER          (0x06)
#define PN532_COMMAND_WRITEREGISTER         (0x08)
#define PN532_COMMAND_READGPIO              (0x0C)
#define PN532_COMMAND_WRITEGPIO             (0x0E)
#define PN532_COMMAND_SETSERIALBAUDRATE     (0x10)
#define PN532_COMMAND_SETPARAMETERS         (0x12)
#define PN532_COMMAND_POWERDOWN             (0x16)
#define PN532_COMMAND_RFCONFIGURATION       (0x32)
#define PN532_COMMAND_RFREGULATIONTEST      (0x58)
#define PN532_COMMAND_INJUMPFORDEP          (0x56)
#define PN532_COMMAND_INJUMPFORPSL          (0x46)

#define PN532_COMMAND_INATR                 (0x50)
#define PN532_COMMAND_INPSL                 (0x4E)
#define PN532_COMMAND_INDESELECT            (0x44)
#define PN532_COMMAND_INRELEASE             (0x52)
#define PN532_COMMAND_INSELECT              (0x54)
#define PN532_COMMAND_INAUTOPOLL            (0x60)
#define PN532_COMMAND_TGINITASTARGET        (0x8C)
#define PN532_COMMAND_TGSETGENERALBYTES     (0x92)
#define PN532_COMMAND_TGGETDATA             (0x86)
#define PN532_COMMAND_TGSETDATA             (0x8E)
#define PN532_COMMAND_TGSETMETADATA         (0x94)
#define PN532_COMMAND_TGGETINITIATORCOMMAND (0x88)
#define PN532_COMMAND_TGRESPONSETOINITIATOR (0x90)
#define PN532_COMMAND_TGGETTARGETSTATUS     (0x8A)

	enum STATUS_CODE {
		STATUS_CHECKSUMERROR = 0xfa,
		STATUS_I2CREADY_TIMEOUT,
		STATUS_WRONG_ACK,
		STATUS_WRONG_PREAMBLE
	};
	static const byte PACKBUFFSIZE = 64;
//
	byte pin_irq; // P70_IRQ
	byte pin_rst;
	byte i2c_addr;
	//
	byte chksum;
	byte packet[PACKBUFFSIZE];
	byte lastStatus;
	//
	union {
		byte IDm[8];
		struct {
			byte IDLength;
			byte UID[7];
		};
	} target;

	inline void wirewrite(const byte & d) {
		Wire.write(d);
	}

	inline byte wireread() {
		return Wire.read();
	}

	void send(byte d);
	void sendcc(byte *buf, byte n);
	byte receive();
	byte receive(byte * buf, int n);
	byte receive(byte * buf);
	boolean checkACKframe(long timeout = 1000);
	byte IRQ_status(void);
	boolean IRQ_wait(long timeout = 1000);

//	boolean sendCommand(byte cmd, long timeout = 1000);
public:
	static const byte I2C_ADDRESS = (0x48 >> 1);

	// Mifare Commands
#define MIFARE_CMD_AUTH_A                   (0x60)
#define MIFARE_CMD_AUTH_B                   (0x61)
#define MIFARE_CMD_READ                     (0x30)
#define MIFARE_CMD_WRITE                    (0xA0)
#define MIFARE_CMD_TRANSFER                 (0xB0)
#define MIFARE_CMD_DECREMENT                (0xC0)
#define MIFARE_CMD_INCREMENT                (0xC1)
#define MIFARE_CMD_STORE                    (0xC2)
//
	static const byte FELICA_CMD_COMMTHRUEX = 0xa0;
	static const byte FELICA_CMD_ECHO = 0xf0;
	static const byte FELICA_CMD_POLLING = 0x00;
	static const byte FELICA_CMD_REQUESTSERVICE = 0x02;
	static const byte FELICA_CMD_ERQUESTRESPONSE = 0x04;
	static const byte FELICA_CMD_READWITHOUTENCRYPTION = 0x06;
	static const byte FELICA_CMD_WRITEWITHOUTENCRYPTION = 0x08;
	static const byte FELICA_CMD_REQUESTSYSTEMCODE = 0x0c;

	static void printArray(const byte * a, byte len) {
		for (int i = 0; i < len; i++) {
			Serial.print(a[i] >> 4 & 0x0f, HEX);
			Serial.print(a[i] & 0x0f, HEX);
			Serial.print(" ");
		}
	}

public:

	PN532(byte addr = I2C_ADDRESS, byte irq = 0xff, byte rst = 0xff);

	void init();
	inline void begin() {
		init();
	}

	unsigned long getFirmwareVersion();
	boolean SAMConfiguration(byte mode = 0x01, byte timeout = 0x14,
			byte use_irq = 0x01);

	static const byte BaudrateType_106kbitTypeA = 0x00;
	static const byte BaudrateType_212kbitFeliCa = 0x01;
<<<<<<< HEAD:libraries/RFID_PN532_I2C/PN532_I2C.h

=======
	byte InListPassiveTarget(const byte maxtg, const byte brty,
			const byte initlen = 0, byte * data = NULL, const long & wmillis = 100);
>>>>>>> PN532:libraries/RFID_NFC/PN532_I2C.h
	static const byte Type_GenericPassiveTypeA = 0x00;
	static const byte Type_GenericPassive212kbFeliCa = 0x01;
	static const byte Type_GenericPassive424kbFeliCa = 0x02;
	static const byte Type_PassiveTypeB = 0x03;
	static const byte Type_Mifare = 0x10;
	static const byte Type_FeliCa212kb = 0x11;
	static const byte Type_FeliCa424kb = 0x12;

	byte InListPassiveTarget(const byte maxtg, const byte brty, byte * data,
			const byte initlen, const long & wmillis = 100);
	byte InAutoPoll(const byte pollnr, const byte per, const byte * types,
			const byte typeslen);

	inline byte autoPoll(const byte polltypes[], byte * resp, const long & waitmillis = 1000) {
		if ( InAutoPoll(1,2, polltypes+1, polltypes[0])
			&& getCommandResponse(PN532::COMMAND_InAutoPoll, resp, waitmillis))
			return resp[0];
		return 0;
	}

	byte InDataExchange(const byte Tg, const byte * data, const byte length);
//	byte InDataExchange(const byte Tg, const byte fcmd, const byte * data, const byte len);
	byte InDataExchange(const byte Tg, const byte micmd, const byte blkaddr,
			const byte * data, const byte datalen);

	byte getCommandResponse(const byte, byte * resp, const long & waitmillis =
			1000);
	byte listPassiveTarget(byte * data, const byte brty =
			BaudrateType_106kbitTypeA, const word syscode = 0xffff);

	byte mifare_AuthenticateBlock(const byte * uid, byte uidLen,
			word blockNumber, const byte * keyData);
	byte mifare_ReadDataBlock(uint8_t blockNumber, uint8_t * data);

	byte InCommunicateThru(const byte * data, const byte len);
	byte getCommunicateThruResponse(byte * data);

	//	byte felica_DataExchange(const byte cmd, const byte * data, const byte len);
	//	byte felica_getDataExchangeResponse(const byte fcmd, byte * resp);
	byte felica_Polling(byte * resp, const word syscode = 0xffff);
	byte felica_RequestService(byte * resp, const word servcodes[],
			const byte servnum);
	word felica_RequestService(const word servcode);
	byte felica_RequestSystemCode(byte * resp);
	// Block list accepts only two byte codes.
	byte felica_ReadWithoutEncryption(byte * resp, const word servcode,
			const byte blknum, const word blklist[]);

};

#endif /* PN532_I2C_H_ */
