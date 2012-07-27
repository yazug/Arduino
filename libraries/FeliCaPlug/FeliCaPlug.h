/*
 * FeliCaPlug library for Arduino
 *
 * Copyright 2010 Sony Corporation
 */

#include <inttypes.h>

#ifndef FELICA_PLUG_H_
#define FELICA_PLUG_H_

/* --------------------------------
 * Constant
 * -------------------------------- */

#define FELICA_PLUG_SW_PIN          9
#define FELICA_PLUG_SEL_PIN         10
#define FELICA_PLUG_DATA_PIN        11
#define FELICA_PLUG_SPICLK_PIN      12
#define FELICA_PLUG_IRQ_PIN         2
#define FELICA_PLUG_RFDET_PIN       3

#define FELICA_PLUG_LED_PIN         13
#define FELICA_PLUG_LED_PIN_NONE    0xff

#define FELICA_PLUG_NO_RF_SUSPEND_TIME      3000 /* ms */
#define FELICA_PLUG_NO_ACTION_INTERVAL      0  /* ms */

#define FELICA_PLUG_MODE_FB         0x18
#define FELICA_PLUG_MODE_FT         0x1a
#define FELICA_PLUG_MODE_TYPE3      0x1b
#define FELICA_PLUG_MODE_NONE       0x00

#define FELICA_PLUG_PMM_READ        0xff
#define FELICA_PLUG_PMM_WRITE       0xff

#define FELICA_PLUG_EEPROM_MAX_NUM_OF_BLOCKS    31

/* --------------------------------
 * Type and Structure
 * -------------------------------- */

typedef void (*FeliCaPlug_common_callback_t)(void);

typedef void (*FeliCaPlug_FB_callback_t)(
    uint8_t* numOfBlocks,
    const uint8_t* blockList,
    uint8_t* blockData);

typedef void (*FeliCaPlug_FT_callback_t)(
    uint16_t blockNum);

/* --------------------------------
 * Class Declaration
 * -------------------------------- */

class FeliCaPlug
{
public:
    void initPort(
        uint8_t swPin = FELICA_PLUG_SW_PIN,
        uint8_t selPin = FELICA_PLUG_SEL_PIN,
        uint8_t dataPin = FELICA_PLUG_DATA_PIN,
        uint8_t spiclkPin = FELICA_PLUG_SPICLK_PIN,
        uint8_t irqPin = FELICA_PLUG_IRQ_PIN,
        uint8_t rfdetPin = FELICA_PLUG_RFDET_PIN);
    void setParam(
        uint8_t mode,
        uint8_t dfc[2],
        uint8_t userParam[4],
        uint8_t pmmRead = FELICA_PLUG_PMM_READ,
        uint8_t pmmWrite = FELICA_PLUG_PMM_WRITE);
    void setLEDPin(uint8_t ledPin);
    int setBlockData(
        uint8_t* blockData,
        uint16_t numOfBlocks,
        int readOnly = 0,
        int syncWithEEPROM = 0);
    void setSuspendCallback(FeliCaPlug_common_callback_t func);
    void setResumeCallback(FeliCaPlug_common_callback_t func);
    void setFBCallback(FeliCaPlug_FB_callback_t func);
    void setFTReadCallback(FeliCaPlug_FT_callback_t func);
    void setFTWriteCallback(FeliCaPlug_FT_callback_t func);

    void doLoop(unsigned long noActionInterval =
                FELICA_PLUG_NO_ACTION_INTERVAL);

private:
    void changeLED(int on);

    void initDevice(void);
    void execCmdFB(void);
    void execCmdFT(void);
    void execCmdFTRead(void);
    void execCmdFTWrite(void);
    uint8_t checkReadBlock(
        uint16_t blockNum,
        uint8_t* blockData);
    void readBlock(
        uint16_t blockNum,
        uint8_t* readBlockData);
    uint8_t checkWriteBlock(
        uint16_t blockNum,
        const uint8_t* blockData);
    void writeBlock(
        uint16_t blockNum,
        const uint8_t* blockData);

    void suspend(void);
    void resume(void);
    void beginSend(void);
    void endSend(void);
    void sendByte(uint8_t data);
    void sendByteArray(
        const uint8_t* data,
        uint8_t length);
    uint8_t receiveByte(void);
    void receiveByteArray(
        uint8_t* data,
        uint8_t length);
    void updateLastRFDetTime(void);

    int readBlockDataFromEEPROM(void);
    void writeBlockDataToEEPROM(void);
    void writeEEPROMIfChanged(
        int addr,
        uint8_t value);

private:
    uint8_t swPin;
    uint8_t selPin;
    uint8_t dataPin;
    uint8_t spiclkPin;
    uint8_t irqPin;
    uint8_t rfdetPin;

    uint8_t ledPin;

    uint8_t mode;
    int active;

    uint8_t dfc[2];
    uint8_t userParam[4];
    uint8_t pmmRead;
    uint8_t pmmWrite;

    unsigned long lastRFDetTime;
    unsigned long noRFSuspendTime;

    uint8_t* blockData;
    uint16_t numOfBlocks;
    int readOnly;
    int syncWithEEPROM;

    FeliCaPlug_common_callback_t suspendCallback;
    FeliCaPlug_common_callback_t resumeCallback;
    FeliCaPlug_FB_callback_t fbCallback;
    FeliCaPlug_FT_callback_t ftReadCallback;
    FeliCaPlug_FT_callback_t ftWriteCallback;
};

#endif /* !FELICA_PLUG_H_ */
