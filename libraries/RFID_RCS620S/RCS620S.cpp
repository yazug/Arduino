/*
 * RC-S620/S sample library for Arduino
 *
 * Copyright 2010 Sony Corporation
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <Wprogram.h>
#include <Print.h>
#include <HardwareSerial.h>
#endif

#include <SoftwareSerial.h>

#include "RCS620S.h"

/* --------------------------------
 * Constant
 * -------------------------------- */

#define RCS620S_DEFAULT_TIMEOUT  1000

/* --------------------------------
 * Variable
 * -------------------------------- */

/* --------------------------------
 * Prototype Declaration
 * -------------------------------- */

/* --------------------------------
 * Macro
 * -------------------------------- */

/* --------------------------------
 * Function
 * -------------------------------- */

/* ------------------------
 * public
 * ------------------------ */

RCS620S::RCS620S() :
		port(Serial), portType(HARDWARESERIAL) {
	this->timeout = RCS620S_DEFAULT_TIMEOUT;
}

RCS620S::RCS620S(Stream & ser) :
		port(ser), portType(SOFTWARESERIAL) {
	this->timeout = RCS620S_DEFAULT_TIMEOUT;
}

int RCS620S::initDevice(void) {
	int ret;
	uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];
	uint16_t responseLen;

	static_cast<HardwareSerial &>(port).begin(115200);

	/* RFConfiguration (various timings) */
	ret = rwCommand((const uint8_t*) "\xd4\x32\x02\x00\x00\x00", 6, response,
			&responseLen);
	if (!ret || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
		Serial.println("RF Config. failed.");
		return 0;
	}

	/* RFConfiguration (max retries) */
	ret = rwCommand((const uint8_t*) "\xd4\x32\x05\x00\x00\x00", 6, response,
			&responseLen);
	if (!ret || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
		Serial.println("RF Config. failed 2.");
		return 0;
	}

	/* RFConfiguration (additional wait time = 24ms) */
	ret = rwCommand((const uint8_t*) "\xd4\x32\x81\xb7", 4, response,
			&responseLen);
	if (!ret || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
		Serial.println("RF Config. failed 3.");
		return 0;
	}

	return 1;
}

int RCS620S::polling(uint16_t systemCode) {
	int ret;
	uint8_t buf[9];
	uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];
	uint16_t responseLen;

	/* InListPassiveTarget */
	memcpy(buf, "\xd4\x4a\x01\x01\x00\xff\xff\x00\x00", 9);
	buf[5] = (uint8_t) ((systemCode >> 8) & 0xff);
	buf[6] = (uint8_t) ((systemCode >> 0) & 0xff);

	ret = rwCommand(buf, 9, response, &responseLen);
	if (!ret || (responseLen != 22)
			|| (memcmp(response, "\xd5\x4b\x01\x01\x12\x01", 6) != 0)) {
		return 0;
	}

	memcpy(this->idm, response + 6, 8);
	memcpy(this->pmm, response + 14, 8);

	return 1;
}

int RCS620S::cardCommand(const uint8_t* command, uint8_t commandLen,
		uint8_t response[RCS620S_MAX_CARD_RESPONSE_LEN], uint8_t* responseLen) {
	int ret;
	uint16_t commandTimeout;
	uint8_t buf[RCS620S_MAX_RW_RESPONSE_LEN];
	uint16_t len;

	if (this->timeout >= (0x10000 / 2)) {
		commandTimeout = 0xffff;
	} else {
		commandTimeout = (uint16_t) (this->timeout * 2);
	}

	/* CommunicateThruEX */
	buf[0] = 0xd4;
	buf[1] = 0xa0;
	buf[2] = (uint8_t) ((commandTimeout >> 0) & 0xff);
	buf[3] = (uint8_t) ((commandTimeout >> 8) & 0xff);
	buf[4] = (uint8_t) (commandLen + 1);
	memcpy(buf + 5, command, commandLen);

	ret = rwCommand(buf, 5 + commandLen, buf, &len);
	if (!ret || (len < 4) || (buf[0] != 0xd5) || (buf[1] != 0xa1)
			|| (buf[2] != 0x00) || (len != (3 + buf[3]))) {
		return 0;
	}

	*responseLen = (uint8_t) (buf[3] - 1);
	memcpy(response, buf + 4, *responseLen);

	return 1;
}

int RCS620S::rfOff(void) {
	int ret;
	uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN];
	uint16_t responseLen;

	/* RFConfiguration (RF field) */
	ret = rwCommand((const uint8_t*) "\xd4\x32\x01\x00", 4, response,
			&responseLen);
	if (!ret || (responseLen != 2) || (memcmp(response, "\xd5\x33", 2) != 0)) {
		return 0;
	}

	return 1;
}

int RCS620S::push(const uint8_t* data, uint8_t dataLen) {
	int ret;
	uint8_t buf[RCS620S_MAX_CARD_RESPONSE_LEN];
	uint8_t responseLen;

	if (dataLen > 224) {
		return 0;
	}

	/* Push */
	buf[0] = 0xb0;
	memcpy(buf + 1, this->idm, 8);
	buf[9] = dataLen;
	memcpy(buf + 10, data, dataLen);

	ret = cardCommand(buf, 10 + dataLen, buf, &responseLen);
	if (!ret || (responseLen != 10) || (buf[0] != 0xb1)
			|| (memcmp(buf + 1, this->idm, 8) != 0) || (buf[9] != dataLen)) {
		return 0;
	}

	buf[0] = 0xa4;
	memcpy(buf + 1, this->idm, 8);
	buf[9] = 0x00;

	ret = cardCommand(buf, 10, buf, &responseLen);
	if (!ret || (responseLen != 10) || (buf[0] != 0xa5)
			|| (memcmp(buf + 1, this->idm, 8) != 0) || (buf[9] != 0x00)) {
		return 0;
	}

	delay(1000);

	return 1;
}

/* ------------------------
 * private
 * ------------------------ */

int RCS620S::rwCommand(const uint8_t* command, uint16_t commandLen,
		uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN], uint16_t* responseLen) {
	int ret;
	uint8_t buf[9];

	flushSerial();

	uint8_t dcs = calcDCS(command, commandLen);

	/* transmit the command */
	buf[0] = 0x00;
	buf[1] = 0x00;
	buf[2] = 0xff;
	if (commandLen <= 255) {
		/* normal frame */
		buf[3] = commandLen;
		buf[4] = (uint8_t) -buf[3];
		writeSerial(buf, 5);
	} else {
		/* extended frame */
		buf[3] = 0xff;
		buf[4] = 0xff;
		buf[5] = (uint8_t) ((commandLen >> 8) & 0xff);
		buf[6] = (uint8_t) ((commandLen >> 0) & 0xff);
		buf[7] = (uint8_t) -(buf[5] + buf[6]);
		writeSerial(buf, 8);
	}
	writeSerial(command, commandLen);
	buf[0] = dcs;
	buf[1] = 0x00;
	writeSerial(buf, 2);

	/* receive an ACK */
	ret = readSerial(buf, 6);

	if (ret == 0 || (memcmp(buf, "\x00\x00\xff\x00\xff\x00", 6) != 0)) {
		cancel();
		Serial.println("canceled.");
		return 0;
	}

	/* receive a response */
	ret = readSerial(buf, 5);
	if (!ret) {
		cancel();
		return 0;
	} else if (memcmp(buf, "\x00\x00\xff", 3) != 0) {
		return 0;
	}
	if ((buf[3] == 0xff) && (buf[4] == 0xff)) {
		ret = readSerial(buf + 5, 3);
		if (!ret || (((buf[5] + buf[6] + buf[7]) & 0xff) != 0)) {
			return 0;
		}
		*responseLen = (((uint16_t) buf[5] << 8) | ((uint16_t) buf[6] << 0));
	} else {
		if (((buf[3] + buf[4]) & 0xff) != 0) {
			return 0;
		}
		*responseLen = buf[3];
	}
	if (*responseLen > RCS620S_MAX_RW_RESPONSE_LEN) {
		return 0;
	}

	ret = readSerial(response, *responseLen);
	if (!ret) {
		cancel();
		return 0;
	}

	dcs = calcDCS(response, *responseLen);

	ret = readSerial(buf, 2);
	if (!ret || (buf[0] != dcs) || (buf[1] != 0x00)) {
		cancel();
		return 0;
	}

	return 1;
}

void RCS620S::cancel(void) {
	/* transmit an ACK */
	writeSerial((const uint8_t*) "\x00\x00\xff\x00\xff\x00", 6);
	delay(1);
	flushSerial();
}

uint8_t RCS620S::calcDCS(const uint8_t* data, uint16_t len) {
	uint8_t sum = 0;

	for (uint16_t i = 0; i < len; i++) {
		sum += data[i];
	}

	return (uint8_t) -(sum & 0xff);
}

void RCS620S::writeSerial(const uint8_t* data, uint16_t len) {
	port.write(data, len);
}

int RCS620S::readSerial(uint8_t* data, uint16_t len) {
	uint16_t nread = 0;
	unsigned long t0 = millis();

	while (nread < len) {
		if (checkTimeout(t0)) {
			return 0;
		}
		if (port.available() > 0) {
			data[nread] = port.read();
			nread++;
		}
	}

	return nread;
}

void RCS620S::flushSerial(void) {
	port.flush();
}

int RCS620S::checkTimeout(unsigned long t0) {
	unsigned long t = millis();

	if ((t - t0) >= this->timeout) {
		return 1;
	}

	return 0;
}
