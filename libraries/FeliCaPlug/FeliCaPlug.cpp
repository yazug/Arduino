/*
 * FeliCaPlug library for Arduino
 *
 * Copyright 2010 Sony Corporation
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Wprogram.h"
#include "Print.h"
#include "EEPROM.h"

#include "FeliCaPlug.h"

/* --------------------------------
 * Constant
 * -------------------------------- */

#define FELICA_PLUG_CMD_READ_WO_ENC         0x06
#define FELICA_PLUG_CMD_WRITE_WO_ENC        0x08

#define FELICA_PLUG_STATUS_FLAG_INVALID_BLOCK_SERV  0xa3
#define FELICA_PLUG_STATUS_FLAG_PERMISSION_ERROR    0xa5
#define FELICA_PLUG_STATUS_FLAG_INVALID_SERVICE     0xa6
#define FELICA_PLUG_STATUS_FLAG_INVALID_BLOCK_NUM   0xa8

#define FELICA_PLUG_EEPROM_MAGIC0   0x46 /* F */
#define FELICA_PLUG_EEPROM_MAGIC1   0x50 /* P */

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

void FeliCaPlug::initPort(
    uint8_t swPin,
    uint8_t selPin,
    uint8_t dataPin,
    uint8_t spiclkPin,
    uint8_t irqPin,
    uint8_t rfdetPin)
{
    this->swPin = swPin;
    this->selPin = selPin;
    this->dataPin = dataPin;
    this->spiclkPin = spiclkPin;
    this->irqPin = irqPin;
    this->rfdetPin = rfdetPin;

    this->ledPin = FELICA_PLUG_LED_PIN_NONE;
    this->noRFSuspendTime = FELICA_PLUG_NO_RF_SUSPEND_TIME;

    this->mode = FELICA_PLUG_MODE_NONE;
    this->active = 0;

    this->blockData = NULL;
    this->numOfBlocks = 0;
    this->readOnly = 0;
    this->syncWithEEPROM = 0;

    this->suspendCallback = NULL;
    this->resumeCallback = NULL;
    this->fbCallback = NULL;
    this->ftReadCallback = NULL;
    this->ftWriteCallback = NULL;

    pinMode(this->swPin, OUTPUT);
    pinMode(this->selPin, OUTPUT);
    pinMode(this->dataPin, INPUT);
    pinMode(this->spiclkPin, OUTPUT);

    pinMode(this->irqPin, INPUT);
    pinMode(this->rfdetPin, INPUT);

    suspend();
    digitalWrite(this->selPin, LOW);
    digitalWrite(this->spiclkPin, HIGH);

    delayMicroseconds(100); /* wait 100usec after Plug's VDD on */
}

void FeliCaPlug::setParam(
    uint8_t mode,
    uint8_t dfc[2],
    uint8_t userParam[4],
    uint8_t pmmRead,
    uint8_t pmmWrite)
{
    this->mode = mode;
    memcpy(this->dfc, dfc, 2);
    memcpy(this->userParam, userParam, 4);
    this->pmmRead = pmmRead;
    this->pmmWrite = pmmWrite;
}

void FeliCaPlug::setLEDPin(uint8_t ledPin)
{
    if (ledPin != FELICA_PLUG_LED_PIN_NONE) {
        pinMode(ledPin, OUTPUT);
        digitalWrite(ledPin, LOW);
    }
    this->ledPin = ledPin;
}

int FeliCaPlug::setBlockData(
    uint8_t* blockData,
    uint16_t numOfBlocks,
    int readOnly,
    int syncWithEEPROM)
{
    int ret = 1; /* sucess */

    this->blockData = blockData;
    this->numOfBlocks = numOfBlocks;
    this->readOnly = readOnly;
    this->syncWithEEPROM = syncWithEEPROM;
    if (syncWithEEPROM) {
        ret = readBlockDataFromEEPROM();
    }

    return ret;
}

void FeliCaPlug::setSuspendCallback(FeliCaPlug_common_callback_t func)
{
    this->suspendCallback = func;
}

void FeliCaPlug::setResumeCallback(FeliCaPlug_common_callback_t func)
{
    this->resumeCallback = func;
}

void FeliCaPlug::setFBCallback(FeliCaPlug_FB_callback_t func)
{
    this->fbCallback = func;
}

void FeliCaPlug::setFTReadCallback(FeliCaPlug_FT_callback_t func)
{
    this->ftReadCallback = func;
}

void FeliCaPlug::setFTWriteCallback(FeliCaPlug_FT_callback_t func)
{
    this->ftWriteCallback = func;
}

void FeliCaPlug::doLoop(unsigned long noActionInterval)
{
    if (digitalRead(this->rfdetPin) == LOW) {
        updateLastRFDetTime();

        if (!(this->active)) {
            resume();
            return;
        }
    }

    if (this->active) {
        if (digitalRead(this->irqPin) == HIGH) {
            if ((this->mode == FELICA_PLUG_MODE_FT) ||
                (this->mode == FELICA_PLUG_MODE_TYPE3)) {
                execCmdFT();
            } else if (this->mode == FELICA_PLUG_MODE_FB) {
                execCmdFB();
            }
            return;
        }

        if ((millis() - this->lastRFDetTime) > this->noRFSuspendTime) {
            if (((this->mode == FELICA_PLUG_MODE_FT) ||
                 (this->mode == FELICA_PLUG_MODE_TYPE3)) &&
                this->syncWithEEPROM) {
                writeBlockDataToEEPROM();
            }
            suspend();
            return;
        }
    }

    delay(noActionInterval);
}

/* ------------------------
 * private
 * ------------------------ */

void FeliCaPlug::changeLED(int on)
{
    if (this->ledPin != FELICA_PLUG_LED_PIN_NONE) {
        if (on) {
            digitalWrite(this->ledPin, HIGH);
        } else {
            digitalWrite(this->ledPin, LOW);
        }
    }
}

void FeliCaPlug::initDevice(void)
{
    beginSend();
    sendByte(this->mode);
    sendByte(this->pmmRead);
    sendByte(this->pmmWrite);
    sendByteArray(this->dfc, 2);
    sendByteArray(this->userParam, 4);
    endSend();
}

void FeliCaPlug::execCmdFB(void)
{
    uint8_t nblocks;
    uint8_t p;
    uint8_t blockList[12 * 3];
    uint8_t blockData[12 * 16];

    nblocks = receiveByte();
    int i;

    p = 0;
    for (i = 0; i < nblocks; i++) {
        blockList[p] = receiveByte();
        if (!(blockList[p] & 0x80)) {
            p++;
            blockList[p] = receiveByte();
        }
        p++;
        blockList[p] = receiveByte();
        p++;
    }
    receiveByteArray(blockData, nblocks * 16);

    if (this->fbCallback != NULL) {
        this->fbCallback(&nblocks, blockList, blockData);
    }

    beginSend();
    sendByteArray(blockData, nblocks * 16);
    endSend();
}

void FeliCaPlug::execCmdFT(void)
{
    uint8_t cmd;

    cmd = receiveByte();
    if (cmd == FELICA_PLUG_CMD_READ_WO_ENC) {
        execCmdFTRead();
    } else {
        execCmdFTWrite();
    }
}

void FeliCaPlug::execCmdFTRead(void)
{
    uint8_t nblocks;
    uint8_t b0;
    uint16_t blockNum[12];
    uint8_t blockData[12 * 16];
    uint8_t statusFlag1 = 0;
    uint8_t statusFlag2 = 0;
    int i;

    nblocks = receiveByte();
    for (i = 0; i < nblocks; i++) {
        b0 = receiveByte();
        if (b0 & 0x80) {
            blockNum[i] = receiveByte();
        } else {
            blockNum[i] = receiveByte();
            blockNum[i] |= ((uint16_t)receiveByte() << 8);
        }
        if ((statusFlag1 == 0x00) &&
            ((b0 & 0x7f) != 0x00)) {
            statusFlag1 |= (1 << (i % 8));
            statusFlag2 = FELICA_PLUG_STATUS_FLAG_INVALID_BLOCK_SERV;
        }
        if (statusFlag1 == 0x00) {
            statusFlag2 = checkReadBlock(blockNum[i], blockData + (i * 16));
            if (statusFlag2 != 0x00) {
                statusFlag1 |= (1 << (i % 8));
            }
        }
    }

    if (statusFlag1 == 0x00) {
        for (i = 0; i < nblocks; i++) {
            readBlock(blockNum[i], blockData + (i * 16));
        }
    }

    if (statusFlag1 == 0x00) {
        beginSend();
        sendByte(statusFlag1);
        sendByte(statusFlag2);
        sendByteArray(blockData, nblocks * 16);
        endSend();
    } else {
        /* no response, reinitialize the device */
        digitalWrite(this->swPin, LOW);
        delayMicroseconds(1);
        digitalWrite(this->swPin, HIGH);
        delayMicroseconds(50);

        initDevice();
    }
}

void FeliCaPlug::execCmdFTWrite(void)
{
    uint8_t nblocks;
    uint8_t b0;
    uint16_t blockNum[12];
    uint8_t blockData[12 * 16];
    uint8_t statusFlag1 = 0;
    uint8_t statusFlag2 = 0;
    int i;

    if (this->readOnly) {
        statusFlag1 = 0xff;
        statusFlag2 = FELICA_PLUG_STATUS_FLAG_INVALID_SERVICE;
    }

    nblocks = receiveByte();
    for (i = 0; i < nblocks; i++) {
        b0 = receiveByte();
        if (b0 & 0x80) {
            blockNum[i] = receiveByte();
        } else {
            blockNum[i] = receiveByte();
            blockNum[i] |= ((uint16_t)receiveByte() << 8);
        }
        if ((statusFlag1 == 0x00) &&
            ((b0 & 0x7f) != 0x00)) {
            statusFlag1 |= (1 << (i % 8));
            statusFlag2 = FELICA_PLUG_STATUS_FLAG_INVALID_BLOCK_SERV;
        }
    }
    receiveByteArray(blockData, nblocks * 16);

    for (i = 0; i < nblocks; i++) {
        if (statusFlag1 == 0x00) {
            statusFlag2 = checkWriteBlock(blockNum[i], blockData + (i * 16));
            if (statusFlag2 != 0x00) {
                statusFlag1 |= (1 << (i % 8));
            }
        }
    }
    if (statusFlag1 == 0x00) {
        for (i = 0; i < nblocks; i++) {
            writeBlock(blockNum[i], blockData + (i * 16));
        }
    }

    beginSend();
    sendByte(statusFlag1);
    sendByte(statusFlag2);
    endSend();
}

uint8_t FeliCaPlug::checkReadBlock(
    uint16_t blockNum,
    uint8_t* blockData)
{
    if (blockNum >= this->numOfBlocks) {
        return FELICA_PLUG_STATUS_FLAG_INVALID_BLOCK_NUM;
    }

    return 0;
}

void FeliCaPlug::readBlock(
    uint16_t blockNum,
    uint8_t* blockData)
{
    if (this->ftReadCallback != NULL) {
        this->ftReadCallback(blockNum);
    }
    if (this->blockData != NULL) {
        memcpy(blockData, this->blockData + (blockNum * 16), 16);
    } else {
        memset(blockData, 0, 16);
    }
}

uint8_t FeliCaPlug::checkWriteBlock(
    uint16_t blockNum,
    const uint8_t* blockData)
{
    if (blockNum >= this->numOfBlocks) {
        return FELICA_PLUG_STATUS_FLAG_INVALID_BLOCK_NUM;
    }
    if (this->blockData == NULL) {
        return FELICA_PLUG_STATUS_FLAG_PERMISSION_ERROR;
    }

    return 0;
}

void FeliCaPlug::writeBlock(
    uint16_t blockNum,
    const uint8_t* blockData)
{
    if (this->blockData != NULL) {
        memcpy(this->blockData + (blockNum * 16), blockData, 16);
    }
    if (this->ftWriteCallback != NULL) {
        this->ftWriteCallback(blockNum);
    }
}

void FeliCaPlug::suspend(void)
{
    digitalWrite(this->swPin, LOW);
    this->active = 0;
    changeLED(this->active);

    if (this->suspendCallback != NULL) {
        this->suspendCallback();
    }
}

void FeliCaPlug::resume(void)
{
    digitalWrite(this->swPin, HIGH);
    delayMicroseconds(50);
    this->active = 1;
    changeLED(this->active);

    initDevice();

    if (this->resumeCallback != NULL) {
        this->resumeCallback();
    }
}

void FeliCaPlug::beginSend(void)
{
    digitalWrite(this->selPin, LOW);
    delayMicroseconds(1);
    pinMode(this->dataPin, OUTPUT);
}

void FeliCaPlug::endSend(void)
{
    delayMicroseconds(1);
    pinMode(this->dataPin, INPUT);
    delayMicroseconds(1);
    digitalWrite(this->selPin, HIGH);
}

void FeliCaPlug::sendByte(uint8_t data)
{
    for (int i = 0; i < 8; i++) {
        digitalWrite(this->spiclkPin, LOW);
        if (data & 0x80) {
            digitalWrite(this->dataPin, HIGH);
        } else {
            digitalWrite(this->dataPin, LOW);
        }
        data <<= 1;
        delayMicroseconds(1);
        digitalWrite(this->spiclkPin, HIGH);
        delayMicroseconds(1);
    }
}

void FeliCaPlug::sendByteArray(
    const uint8_t* data,
    uint8_t length)
{
    for (uint8_t i = 0; i < length; i++) {
        sendByte(data[i]);
    }
}

uint8_t FeliCaPlug::receiveByte(void)
{
    uint8_t data = 0;

    for (uint8_t i = 0; i < 8; i++) {
        digitalWrite(this->spiclkPin, LOW);
        delayMicroseconds(1);
        data = ((data << 1) | (digitalRead(this->dataPin) == HIGH));
        digitalWrite(this->spiclkPin, HIGH);
        delayMicroseconds(1);
    }

    return data;
}

void FeliCaPlug::receiveByteArray(
    uint8_t* data,
    uint8_t length)
{
    for (int i = 0; i < length; i++) {
        data[i] = receiveByte();
    }
}

void FeliCaPlug::updateLastRFDetTime(void)
{
    this->lastRFDetTime = millis();
}

int FeliCaPlug::readBlockDataFromEEPROM(void)
{
    uint16_t numOfBlocks;

    if ((EEPROM.read(0) != FELICA_PLUG_EEPROM_MAGIC0) ||
        (EEPROM.read(1) != FELICA_PLUG_EEPROM_MAGIC1)) {
        return 0;
    }

    numOfBlocks = EEPROM.read(2);
    numOfBlocks |= (EEPROM.read(3) << 8);
    if (numOfBlocks > this->numOfBlocks) {
        return 0;
    }

    for (int i = 0; i < (numOfBlocks * 16); i++) {
        this->blockData[i] = EEPROM.read(16 + i);
    }
    this->numOfBlocks = numOfBlocks;

    return 1; /* success */
}

void FeliCaPlug::writeBlockDataToEEPROM(void)
{
    writeEEPROMIfChanged(0, FELICA_PLUG_EEPROM_MAGIC0);
    writeEEPROMIfChanged(1, FELICA_PLUG_EEPROM_MAGIC1);
    writeEEPROMIfChanged(2, (byte)(this->numOfBlocks & 0xff));
    writeEEPROMIfChanged(3, (byte)((this->numOfBlocks >> 8) & 0xff));

    for (int i = 0; i < (this->numOfBlocks * 16); i++) {
        writeEEPROMIfChanged(16 + i, this->blockData[i]);
    }
}

void FeliCaPlug::writeEEPROMIfChanged(
    int addr,
    uint8_t value)
{
    if (EEPROM.read(addr) != value) {
        EEPROM.write(addr, value);
    }
}
