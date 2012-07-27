/* Arduino WaveHC Library
 * Copyright (C) 2008 by William Greiman
 *  
 * This file is part of the Arduino WaveHC Library
 *  
 * This Library is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with the Arduino WaveHC Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "wiring.h"
#include "SdReader.h"
//
//SPI pin definitions
/** Slave Select pin for card */
#define SS   10
/** spi master output, slave input pin */
#define MOSI 11
/** spi master input, slave output pin */
#define MISO 12
/** spi serial clock pin */
#define SCK  13
//
// inline SPI functions
/** Set Slave Select high */
inline void spiSSHigh(void) {digitalWrite(SS, HIGH);}
/** Set Slave Select low */
inline void spiSSLow(void) {digitalWrite(SS, LOW);}
/** Send a byte to the card */
inline void spiSend(uint8_t b) {SPDR = b; while(!(SPSR & (1 << SPIF)));}
/** Receive a byte from the card */
inline uint8_t spiRec(void) {spiSend(0XFF); return SPDR;}
//
/** status for card in the ready state */
#define R1_READY_STATE 0
/** status for card in the idle state */
#define R1_IDLE_STATE  1
/** start data token for read or write */
#define DATA_START_BLOCK      0XFE
/** mask for data response tokens after a write block operation */
#define DATA_RES_MASK         0X1F
/** write data accepted token */
#define DATA_RES_ACCEPTED     0X05
/** write data crc error token */
#define DATA_RES_CRC_ERROR    0X0B
/** write data programming error token */
#define DATA_RES_WRITE_ERROR  0X0D
//
uint8_t SdReader::cardCommand(uint8_t cmd, uint32_t arg, uint8_t crc)
{
  uint8_t r1;
  // end read if in partialBlockRead mode
  readEnd();
  //select card
  spiSSLow();
  // some cards need extra clocks to go to ready state
  spiRec();
  // send command
  spiSend(cmd | 0x40);
  //send argument
  for (int8_t s = 24; s >= 0; s -= 8) spiSend(arg >> s);
  //send CRC
  spiSend(crc);
  //wait for not busy
#ifdef WHG_MOD
  for (uint8_t retry = 0; (response_ = spiRec()) == 0xFF && retry != 0XFF; retry++);
  return response_;
#else //WHG_MOD
  for (uint8_t retry = 0; (r1 = spiRec()) == 0xFF && retry != 0XFF; retry++);
  return r1;
#endif //WHG_MOD
}
//
#if SD_CARD_INFO_SUPPORT
/**
 * Determine the size of a SD flash memory card.
 * \return The number of 512 byte data blocks in the card
 */ 
uint32_t SdReader::cardSize(void)
{
  csd_t csd;
  if (!readCSD(csd)) return 0;
  if (csd.v1.csd_ver == 0) {
    uint8_t read_bl_len = csd.v1.read_bl_len;
    uint16_t c_size = (csd.v1.c_size_high << 10)
                      | (csd.v1.c_size_mid << 2) | csd.v1.c_size_low;
    uint8_t c_size_mult = (csd.v1.c_size_mult_high << 1) | csd.v1.c_size_mult_low;
    return (uint32_t)(c_size + 1) << (c_size_mult + read_bl_len - 7);
  }
  else if (csd.v2.csd_ver == 1) {
    uint32_t c_size = ((uint32_t)csd.v2.c_size_high << 16)
                      | (csd.v2.c_size_mid << 8) | csd.v2.c_size_low;
    return (c_size + 1) << 10;
  }
  else {
    error(SD_CARD_ERROR_BAD_CSD);
    return 0;
  }
}
#endif //SD_CARD_INFO_SUPPORT
/**
 * Initialize a SD flash memory card.
 * 
* \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure. 
 *
 */  
uint8_t SdReader::init(uint8_t slow)
{
  uint8_t ocr[4];
  pinMode(SS, OUTPUT);
  spiSSHigh();
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  //Enable SPI, Master, clock rate f_osc/128
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
  //must supply min of 74 clock cycles with CS high.
  for (uint8_t i = 0; i < 10; i++) spiSend(0XFF);
  // next two lines prevent re-init hang by some cards (not sure why this works)
  spiSSLow();
  for (uint16_t i = 0; i <= 512; i++) spiRec();
  //send correct crc for CMD0
  uint8_t r = cardCommand(CMD0, 0, 0X95);
  for (uint16_t retry = 0; r != R1_IDLE_STATE; retry++){
    if (retry == 0XFFFF) {
      error(SD_CARD_ERROR_CMD0);
      return 0;
    }
    r = spiRec();
  }
  r = cardCommand(CMD8, 0x1AA, 0X87);
  if (r == 1) {
    uint8_t r7[4];
    for(uint8_t i = 0; i < 4; i++) r7[i] = spiRec();
    type(SD_CARD_TYPE_SD2);
  }
  else if (r & 4) {
    type(SD_CARD_TYPE_SD1);
  }
  else {
    // not a valid SD card
    error(SD_CARD_ERROR_CMD8, r);
    return 0;
  }
  for (uint16_t retry = 0; ; retry++) {
    cardCommand(CMD55, 0);
    r = cardCommand(ACMD41, type() == SD_CARD_TYPE_SD2 ? 0X40000000 : 0);
    if (r == R1_READY_STATE)break;
    if (retry == 1000) {
      error(SD_CARD_ERROR_ACMD41);
      return 0;
    }
  }
  if(cardCommand(CMD58, 0)) {
      error(SD_CARD_ERROR_CMD58);
      return 0;
  }
  for (uint8_t i = 0; i < 4; i++) ocr[i] = spiRec();
  if (type() == SD_CARD_TYPE_SD2 && (ocr[0] & 0XC0) == 0xC0) type(SD_CARD_TYPE_SDHC);
  //use max SPI frequency
  SPCR &= ~((1 << SPR1) | (1 << SPR0)); // f_OSC/4
  if (!slow) SPSR |= (1 << SPI2X); // Doubled Clock Frequency: f_OSC/2
  spiSSHigh();
  return 1;
}
/**
 * Read part of a 512 byte block from a SD card.
 *  
 * \param[in] block Logical block to be read.
 * \param[in] offset Number of bytes to skip at start of block
 * \param[out] dst Pointer to the location that will receive the data. 
 * \param[in] count Number of bytes to read
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.      
 */
uint8_t SdReader::readData(uint32_t block, uint16_t offset, uint8_t *dst, uint16_t count)
{
  if (count == 0) return 1;
  if ((count + offset) > 512) {
    return 0;
  }
  if (!inBlock_ || block != block_ || offset < offset_) {
    block_ = block;
    //use address if not SDHC card
    if (type()!= SD_CARD_TYPE_SDHC) block <<= 9;
    if (cardCommand(CMD17, block)) {
      error(SD_CARD_ERROR_CMD17);
      return 0;
    }
    if (!waitStartBlock()) return 0;
    offset_ = 0;
    inBlock_ = 1;
  }
  //start first spi transfer
  SPDR = 0XFF;
  //skip data before offset
  for (;offset_ < offset; offset_++) {
    while(!(SPSR & (1 << SPIF)));
    SPDR = 0XFF;
  }
  //transfer data
  uint16_t n = count - 1;
  for (uint16_t i = 0; i < n; i++) {
    while(!(SPSR & (1 << SPIF)));
    dst[i] = SPDR;
    SPDR = 0XFF;
  }
  while(!(SPSR & (1 << SPIF)));// wait for last byte
  dst[n] = SPDR;
  offset_ += count;
  if (!partialBlockRead_ || offset_ >= 512) readEnd();
  return 1;
}
 /** Skip remaining data in a block when in partial block read mode. */
void SdReader::readEnd(void)
{
  if (inBlock_) {
    // skip data and crc
    SPDR = 0XFF;
    while (offset_++ < 513) {
      while(!(SPSR & (1 << SPIF)));
      SPDR = 0XFF;
    }
    while(!(SPSR & (1 << SPIF)));//wait for last crc byte
    spiSSHigh();
    inBlock_ = 0;
  }
}
//
#if SD_CARD_INFO_SUPPORT
/** read CID or CSR register */
uint8_t SdReader::readRegister(uint8_t cmd, uint8_t *dst)
{
  if (cardCommand(cmd, 0)) {
    error(SD_CARD_ERROR_READ_REG);
    return 0;
  }
  if(!waitStartBlock()) return 0;
  //transfer data
  for (uint16_t i = 0; i < 16; i++) dst[i] = spiRec();
  spiRec();// get first crc byte
  spiRec();// get second crc byte
  spiSSHigh();
  return 1;
}
#endif //SD_CARD_INFO_SUPPORT
/** Wait for start block token */
uint8_t SdReader::waitStartBlock(void)
{
  uint8_t r;
  uint16_t retry;
  //wait for start of data
  for (retry = 0; ((r = spiRec()) == 0XFF) && retry != 10000; retry++);
  if (r == DATA_START_BLOCK) return 1;
  error(SD_CARD_ERROR_READ, r);
  return 0;
}
