/*
 * PN532_I2C.cpp
 *
 *  Created on: 2012/06/30
 *      Author: sin
 */

#include <Wire.h>
#include "PN532_I2C.h"

//#define PN532DEBUG
//#define MIFAREDEBUG
//#define PN532COMM
//#define FELICADEBUG

PN532::PN532(byte addr, byte irq, byte rst) :
		i2c_addr(addr), pin_irq(irq), pin_rst(rst), lastStatus(0), chksum(0) {
	pinMode(pin_irq, INPUT);
	if (pin_rst != 0xff)
		pinMode(pin_rst, OUTPUT);
	target.IDLength = 0;
}

void PN532::init() {
	chksum = 0;
	lastStatus = 0;
	target.IDLength = 0;
	// Reset the PN532
	if (pin_irq != 0xff) {
		digitalWrite(pin_irq, HIGH);
	}
	if (pin_rst != 0xff) {
		digitalWrite(pin_rst, HIGH);
		digitalWrite(pin_rst, LOW);
		delay(400);
		digitalWrite(pin_rst, HIGH);
	}
}

inline void PN532::send(byte d) {
	wirewrite(d);
	chksum += d;
}

void PN532::sendcc(byte d[], byte len) {
	chksum = 0;
	Wire.beginTransmission(i2c_addr);
	// clk is streched by PN532 to make a pause to resume from power-down
	send(PREAMBLE);
	send(STARTCODE_1);
	send(STARTCODE_2);
	send(len + 1);
	send(~(len + 1) + 1); // LCS, 1-Packet Length Check Sum
	send(HOSTTOPN532); // D4
	for (; len > 0; len--) {
		send(*d++);
	}
	wirewrite(~chksum);
	wirewrite(POSTAMBLE);
	Wire.endTransmission();
}

boolean PN532::checkACKframe(long timeout) {
	// Wait for chip to say its ready!
	if (!IRQ_wait(timeout))
		return false;

	// read acknowledgement
	byte pn532ack[] = { PREAMBLE, STARTCODE_1, STARTCODE_2, 0x00, 0xFF,
			POSTAMBLE };
	receive(packet, 6);
	if (0 != strncmp((char *) packet, (char *) pn532ack, 6)) {
		lastStatus = STATUS_WRONG_ACK;
		return false;
	}
	return true; // ack'd command
}

inline byte PN532::receive() {
	byte d = wireread();
	chksum += d;
	return d;
}

byte PN532::receive(byte *buff, int n) {
	chksum = 0;
	byte i;
	Wire.requestFrom((int) i2c_addr, (int) n);
	receive();
	for (i = 0; i < n; i++) {
		// delayMicroseconds(500);
		*buff++ = receive();
	}
	if (chksum != 0) {
		lastStatus = STATUS_CHECKSUMERROR;
		return 0;
	}
	return i;
}

byte PN532::receive(byte * buff) {
	chksum = 0;
	byte i;
	byte n = 0;

	Wire.requestFrom((int) i2c_addr, BUFFER_LENGTH);
	receive();
	for (i = 0; i < 6; i++) {
		packet[i] = receive();
	}
	n = packet[3];
	if (((packet[3] + packet[4]) & 0xff) != 0x00) {
		lastStatus = STATUS_WRONG_PREAMBLE;
		return 0;
	}
//	Wire.requestFrom((int) i2c_addr, (int) n);
	for (; i < n + 5 + 2; i++) {
		packet[i] = receive();
	}
#ifdef PN532DEBUG
	Serial.print("Len = ");
	Serial.print(n, DEC);
	Serial.print(", ");
	printHexString(packet, n + 7);
	Serial.println();
	Serial.print("xsum: ");
	Serial.print(chksum, HEX);
	Serial.println();
#endif
	if (chksum != 0) {
		lastStatus = STATUS_CHECKSUMERROR;
#ifdef PN532DEBUG
		Serial.println("!!! Checksum error !!!");
		return 0;
#endif
	}
	return n;
}

byte PN532::IRQ_status(void) {
	if (digitalRead(pin_irq) == HIGH)
		return I2C_BUSY;
	else
		return I2C_READY;
}

boolean PN532::IRQ_wait(long timeout) {
	timeout += millis();
	// Wait for chip to say its ready!
	while (digitalRead(pin_irq) == HIGH) {
		if (timeout < millis()) {
			lastStatus = STATUS_I2CREADY_TIMEOUT;
			return false;
		}
		delay(5);
	}
	return true;
}

unsigned long PN532::getFirmwareVersion() {
	unsigned long response;

	packet[0] = COMMAND_GetFirmwareVersion;
	sendcc(packet, 1);
	if (!checkACKframe()) {
#ifdef PN532DEBUG
		Serial.println("Failed to receive ACKframe");
#endif
		return 0;
	}
	// read data packet
	if (!IRQ_wait())
		return 0;
	receive(packet, 6 + 6 + 2); // preamble, d5+(cc+1)+data, checksum, postamble
#ifdef PN532DEBUG
			Serial.println("getFirmwareVersion ");
			PN532::printHexString(packet, 14);
			Serial.println(".");
#endif
	byte pre_sart[] = { PREAMBLE, STARTCODE_1, STARTCODE_2 };
	if (memcmp(packet, pre_sart, 3) != 0)
		return 0;
	if ((packet[3] + packet[4]) & 0xff != 0)
		return 0;
#ifdef PN532DEBUG
	Serial.println("PREAMBLE PASSED.");
#endif

	if (packet[5] != 0xD5 || packet[6] != (COMMAND_GetFirmwareVersion + 1)) {
#ifdef PN532DEBUG
		Serial.println("Firmware doesn't match!");
#endif
		return 0;
	}

	response = *((unsigned long *) &(packet[7]));
	return response;
}

boolean PN532::SAMConfiguration(byte mode, byte timeout, byte use_irq) {
	packet[0] = COMMAND_SAMConfiguration;
	packet[1] = mode; //0x01; // normal mode;
	packet[2] = timeout; //0x14; // timeout 50ms * 20 = 1 second
	packet[3] = use_irq; //0x01; // use IRQ pin!

	sendcc(packet, 4);
	if (!checkACKframe()) {
#ifdef PN532DEBUG
		Serial.println("SAMConfiguration ACK missed.");
#endif
		return false;
	}
	// read data packet
	if (!IRQ_wait())
		return false;
	receive(packet, 6 + 2 + 2);
#ifdef PN532DEBUG
	Serial.print("SAMConfig response: ");
	printHexString(packet, 8);
	Serial.println();
#endif
	return (packet[6] == COMMAND_SAMConfiguration + 1);
}

byte PN532::InListPassiveTarget(const byte maxtg, const byte brty, byte * data,
		const byte initlen, const long & waitmillis) {
//	byte inidatalen = 0;
	packet[0] = COMMAND_InListPassivTarget;
	packet[1] = maxtg; // max 1 cards at once (we can set this to 2 later)
	packet[2] = brty;
	if (initlen > 0) {
		memcpy(packet + 3, data, initlen);
	}
#ifdef PN532DEBUG
	Serial.print("Send in InListPassiveTarget: ");
	printHexString(packet, initlen + 3);
	Serial.println();
#endif
	sendcc(packet, 3 + initlen);
	if (!checkACKframe())
		return 0;
#ifdef PN532DEBUG
	Serial.println("ACKed.");
#endif
	return 1;
}

byte PN532::InAutoPoll(const byte pollnr, const byte period, const byte * types,
		const byte typeslen) {
	byte N = min(max(1, typeslen), 15);
	packet[0] = COMMAND_InAutoPoll;
	packet[1] = pollnr; // no. of polls
	packet[2] = period; // no. of 150 ms period
	// the first type N = 1 mandatory
	memcpy(packet + 3, types, N);
#ifdef PN532DEBUG
	Serial.println("InAutoPoll sending");
	printHexString(packet, typeslen + 3);
	Serial.println();
#endif
	sendcc(packet, 3 + N);
	if (!checkACKframe())
		return 0;

#ifdef PN532DEBUG
	Serial.println("InAutoPoll ACKed.");
#endif
	return 1;
}

byte PN532::getCommandResponse(const byte cmd, byte * resp,
		const long & wmillis) {
	if (!IRQ_wait(wmillis))
		return 0;
	byte count = receive(packet);
//#define PN532DEBUG
#ifdef PN532COMM
	Serial.print(">> ");
	printHexString(packet, count + 7);
	Serial.println();
#endif
//#undef PN532DEBUG
	if (count == 0)
		return 0;
	if (memcmp(packet, "\x00\x00\xff", 3) != 0
			|| ((packet[3] + packet[4]) & 0xff != 0)) {
#ifdef PN532DEBUG
		Serial.println("getCommandResponse illigale preamble.");
#endif
		return 0;
	}
	if (!(packet[5] == 0xd5 && packet[6] == (cmd + 1)))
		return 0;
//	Serial.println("getCommandResponse d5 ok.");
#ifdef PN532DEBUG
	byte xsum = packet[5 + count];
	Serial.print("xsum in data = ");
	Serial.println(xsum, HEX);
#endif
	// checksum is checked in receive.
	if (packet[6 + count] != 0) {
#ifdef PN532DEBUG
		Serial.println("termination 0x00 error");
#endif
		return 0;
	}
#ifdef PN532DEBUG
	Serial.print(packet[6 + count], HEX);
	Serial.println(" getCommandResponse postamble ok.");
#endif

	count -= 2;
	memcpy(resp, packet + 7, count);
	return count;
}

/*
byte PN532::felica_getDataExchangeResponse(const byte fcmd, byte * resp) {
	byte count = getCommandResponse(COMMAND_InDataExchange, resp);
#ifdef FELICADEBUG
	Serial.print("FeliCa >> ");
	printHexString(resp, count);
	Serial.println();
#endif
	lastStatus = resp[0];
	count = resp[1]-2;
	memcpy(resp, resp+3, count);
	return count;
}
*/

byte PN532::listPassiveTarget(byte * data, const byte brty, const word syscode) {
	byte inidatalen = 0;
	switch (brty) {
	case BaudrateType_212kbitFeliCa:
		inidatalen = 5;
		memcpy(data, "\x00\xfe\x00\x00\x00", inidatalen);
		data[1] = syscode&0xff;
		data[2] = syscode>>8;
		break;
	default:
		break;
	}
	if (!InListPassiveTarget(1, brty, data, inidatalen, 50)) {
#ifdef PN532DEBUG
		Serial.println("InListPassiveTarget ACK failed. ");
#endif
		return 0;
	}

	byte count = getCommandResponse(COMMAND_InListPassivTarget, packet);
#ifdef FELICADEBUG
	Serial.print("InListPassiveTarget response: ");
	printHexString(packet, count);
	Serial.println();
	Serial.print("count = ");
	Serial.println(count, DEC);
#endif
	if (count == 0) {
		return 0;
	}
//	count -= 2; // remove checksum and postamble bytes.
	memcpy(data, packet, count);
	return packet[0];

}

byte PN532::InDataExchange(const byte Tg, const byte * data,
		const byte length) {

	// Prepare the authentication command //
	packet[0] = COMMAND_InDataExchange; /* Data Exchange Header */
	packet[1] = Tg; /* target number */
	memcpy(packet + 2, data, length);

#ifdef MIFAREDEBUG
	printHexString(packet, length + 2);
	Serial.println();
#endif
	sendcc(packet, length + 2);
	if (!(checkACKframe())) {
		return 0;
	}
	return 1;
}
/*
byte PN532::InDataExchange(const byte Tg, const byte fcmd, const byte * data,
		const byte len) {
	// Prepare a Felica command //
	packet[0] = COMMAND_InDataExchange; // Data Exchange Header
	packet[1] = Tg; // target number
	packet[2] = len+2;
	packet[3] = fcmd;
	memcpy(packet + 4, data, len);

#ifdef PN532COMM
	Serial.print(">> ");
	printHexString(packet, len + 4);
	Serial.println();
#endif
	sendcc(packet, len + 4);
	if (!(checkACKframe())) {
		return 0;
	}
	return 1;
}
*/

// InDataExchange for Mifare
byte PN532::InDataExchange(const byte Tg, const byte micmd, const byte blkaddr,
		const byte * data, const byte datalen) {
	// Prepare a Mifare command //
	packet[0] = COMMAND_InDataExchange; /* Data Exchange Header */
	packet[1] = Tg; /* target number */
	packet[2] = micmd;
	packet[3] = blkaddr;
	memcpy(packet + 4, data, datalen);

#ifdef MIFAREDEBUG
	Serial.print("Sending in InDataExchange: ");
	printHexString(packet, datalen + 4);
	Serial.println();
#endif
	sendcc(packet, datalen + 4);
	if (!(checkACKframe())) {
		return 0;
	}
	return 1;
}

byte PN532::mifare_AuthenticateBlock(const byte * uid, byte uidLen, word blkn, const byte * keyData) {
	uint8_t len;
	byte tmp[16];

#ifdef MIFAREDEBUG
	Serial.println("In: mifare_AuthenticateBlock");
#endif

	// Prepare the authentication command //
	memcpy(tmp, keyData + 1, 6);
	memcpy(tmp + 6, uid, max(4, uidLen));

	byte authcmd;
	byte rescount;
	switch (keyData[0]) {
	case 0:
	case 0xaa:
	case 0x0a:
	case 'A':
	case 'a':
		authcmd = MIFARE_CMD_AUTH_A;
		break;
	case 1:
	case 0x0b:
	case 0xbb:
	case 'B':
	case 'b':
	default:
		authcmd = MIFARE_CMD_AUTH_B;
		break;
	}

	if (InDataExchange(1, authcmd, blkn, tmp, uidLen + 6)) {
		// Read the response packet
		rescount = getCommandResponse(COMMAND_InDataExchange, packet);
#ifdef MIFAREDEBUG
		Serial.print("mifare_auth Response:");
		printHexString(packet, rescount);
		Serial.println();
#endif
		if (packet[0] == 0) {
			lastStatus = packet[0];
			return 1;
		}
	}
	return 0;
}

byte PN532::mifare_ReadDataBlock(uint8_t blockNumber, uint8_t * data) {
#ifdef MIFAREDEBUG
	Serial.print("Try to read 16 bytes from block ");
	Serial.println(blockNumber);
#endif

	/* Send the command */
	if (!InDataExchange(1, MIFARE_CMD_READ, blockNumber, packet, 0)) {
#ifdef MIFAREDEBUG
		Serial.println("Failed to receive ACK for read command");
#endif
	}
	byte c;
	if (!(c = getCommandResponse(COMMAND_InDataExchange, packet))) {
#ifdef MIFAREDEBUG
		Serial.println("Unexpected response");
		printHexString(packet, 26);
		Serial.println();
#endif
		return 0;
	}
//#define MIFAREDEBUG
#ifdef MIFAREDEBUG
	Serial.print("Packet ");
	printHexString(packet, c);
	Serial.println();
#endif
	if (packet[0] != 0) {
		// error.
		return 0;
	}
	memcpy(data, packet + 1, 16);
#ifdef MIFAREDEBUG
	Serial.print("data ");
	Serial.println(blockNumber);
	printHexString(data, 16);
	Serial.println();
#endif
//#undef MIFAREDEBUG
	return 16;
}

/*
byte PN532::felica_DataExchange(const byte cmd, const byte * data,
		const byte len) {
	// Prepare the command
	packet[0] = COMMAND_InDataExchange;
	packet[1] = 1; // Card number
	packet[2] = len + 2; //2+8; //length of data + 2
	packet[3] = cmd; // FeliCa card command = 0xa0
	memcpy(packet + 4, data, len);

#ifdef FELICADEBUG
	Serial.print("<< ");
	printHexString(packet, len + 4);
	Serial.println();
#endif

	// Send the command
	sendcc(packet, len + 4);
	if (!checkACKframe()) {
		return 0;
	}
	return 1;
}
*/

byte PN532::InCommunicateThru(const byte * data, const byte len) {
	packet[0] = COMMAND_InCommunicateThru;
	packet[1] = len+1;
	memcpy(packet + 2, data, len);

#ifdef FELICADEBUG
	Serial.print("Thru: ");
	Serial.print("count = ");
	Serial.print(len + 1, DEC);
	Serial.print("  ");
	printHexString(packet, len + 2);
	Serial.println();
#endif

	/* Send the command */
	sendcc(packet, len + 2);
	if (!checkACKframe()) {
		return 0;
	}
	return 1;
}

byte PN532::felica_Polling(byte * resp, const word syscode) {
	// Polling command, with system code request.
	resp[0] = FELICA_CMD_POLLING;
	resp[1] = syscode & 0xff;
	resp[2] = syscode >> 8 & 0xff;
	resp[3] = 0x01; // request code: request sys code
	resp[4] = 0; // time slot #
	byte result = InCommunicateThru(resp, 5);
	result = getCommunicateThruResponse(resp);
	if (resp[0] == FELICA_CMD_POLLING + 1 ) {
		memcpy(resp, resp+1, result-9);
		target.IDLength = 8;
		memcpy(target.IDm, resp, result-9);
		return result;
	}
	target.IDLength = 0;
	return 0;
}

byte PN532::getCommunicateThruResponse(byte * data) {
//	InCommunicateThru(data, len);
	/* Read the response packet */
	int count;
	if (!(count = getCommandResponse(COMMAND_InCommunicateThru, packet))) {
		return 0;
	}
#ifdef FELICADEBUG
	Serial.print("Response: ");
	Serial.print("count = ");
	Serial.print(count, DEC);
	Serial.print("   ");
	printHexString(packet, count);
	Serial.println();
#endif
	if ( packet[0] != 0 ) {
		return 0;
	}
	count = packet[1] - 1;
	memcpy(data, packet+2, count);
	return count;
}

byte PN532::felica_RequestService(byte * resp, const word servcodes[],
		const byte servnum) {
	resp[0] = FELICA_CMD_REQUESTSERVICE;
	memcpy(resp + 1, target.IDm, 8);
	resp[9] = servnum;
	for (int i = 0; i < servnum; i++) {
		resp[10 + 2 * i] = servcodes[i] & 0xff;
		resp[11 + 2 * i] = servcodes[i] >> 8 & 0xff;
	}
	byte count = InCommunicateThru(resp, 10 + 2 * servnum);
	count = getCommunicateThruResponse(resp);
	if (resp[0] == FELICA_CMD_REQUESTSERVICE + 1 && count >= 10) {
		byte svnum = resp[9];
		memcpy(resp, resp + 10, svnum * 2);
		return svnum;
	}
	return 0;
}

word PN532::felica_RequestService(const word servcode) {
	byte tmp[14];
	if ( !felica_RequestService(tmp, (word*)&servcode, 1) ) {
		return 0xffff;
	}
	word servcodever = tmp[11];
	return (servcodever<<8) + tmp[10];
}

byte PN532::felica_RequestSystemCode(byte * resp) {
	resp[0] = FELICA_CMD_REQUESTSYSTEMCODE;
	memcpy(resp + 1, target.IDm, 8);
	InCommunicateThru(resp, 9);
	if (getCommunicateThruResponse(resp) == 0)
		return 0;
	byte n = resp[9];
	memcpy(resp, resp + 10, n * 2);
	return n;
}

// Block list accepts only two byte codes.
byte PN532::felica_ReadWithoutEncryption(byte * resp,
		const word servcode, const byte blknum, const word blklist[]) {
	resp[0] = FELICA_CMD_READWITHOUTENCRYPTION;
	memcpy(resp + 1, target.IDm, 8);
	resp[9] = 1;
	resp[10] = servcode & 0xff;
	resp[11] = servcode >> 8 & 0xff;
	resp[12] = blknum;
	byte pos = 13;
	// only two byte array.
	for (int i = 0; i < blknum; i++) {
		// two bytes
		resp[pos++] = (blklist[i] | 0x8000) >> 8 & 0xff ;
		resp[pos++] = blklist[i] & 0xff;
	}
	// pos has been incremented after the last substitution
	byte count = InCommunicateThru(resp, pos);
	count = getCommunicateThruResponse(resp);
	if (resp[9] == 0) {
		byte blocks = resp[11];
		memcpy(resp, resp + 12, blocks * 16);
		return blocks;
	} else {
		return 0;
	}
}
