/*
 * RC-S620/S sample library for Arduino
 *
 * Copyright 2010 Sony Corporation
 */

#ifndef RCS620S_H_
#define RCS620S_H_

#include <inttypes.h>

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <Wprogram.h>
#include <Print.h>
#include <HardwareSerial.h>
#endif

#include <SoftwareSerial.h>


/* --------------------------------
 * Constants
 * -------------------------------- */

#define RCS620S_MAX_CARD_RESPONSE_LEN    254
#define RCS620S_MAX_RW_RESPONSE_LEN      265

/* --------------------------------
 * Class Declaration
 * -------------------------------- */

class RCS620S {
public:
    RCS620S();
    RCS620S(Stream & ser);

    int initDevice(void);
    int polling(uint16_t systemCode = 0xffff);
    int cardCommand(
        const uint8_t* command,
        uint8_t commandLen,
        uint8_t response[RCS620S_MAX_CARD_RESPONSE_LEN],
        uint8_t* responseLen);
    int rfOff(void);

    int push(
        const uint8_t* data,
        uint8_t dataLen);

private:
    int rwCommand(
        const uint8_t* command,
        uint16_t commandLen,
        uint8_t response[RCS620S_MAX_RW_RESPONSE_LEN],
        uint16_t* responseLen);
    void cancel(void);
    uint8_t calcDCS(
        const uint8_t* data,
        uint16_t len);

    void writeSerial(
        const uint8_t* data,
        uint16_t len);
    int readSerial(
        uint8_t* data,
        uint16_t len);
    void flushSerial(void);

    int checkTimeout(unsigned long t0);

public:
    Stream & port;
    const enum {
    	HARDWARESERIAL,
    	SOFTWARESERIAL
    } portType;
    unsigned long timeout;
    uint8_t idm[8];
    uint8_t pmm[8];
};

#endif /* !RCS620S_H_ */
