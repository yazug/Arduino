#ifndef __UARTBRIDGE_H__
#define __UARTBRIDGE_H__

#include "Configuration.h"

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <SPI.h>

// Until we implement a bulk transfer method that takes into account
// the available space in the transmit buffer we will disable bulk transfers.
// The present method of bulk transfers risks overflowing the SPI UART
// transmit buffer.
// Disabling the transfers means we use the standard byte-at-time routine
// from the Print class.
// This isn't a big issue at 9600 baud at least because sending things
// a byte at a time doesn't slow us down noticably.
// TODO: Implement better bulk transfer method
#define ENABLE_BULK_TRANSFERS 0

// SC16IS750 Register definitions
// TODO: Don't bit shift these here, do it in the read/write register routines
#define THR        0x00 << 3
#define RHR        0x00 << 3
#define IER        0x01 << 3
#define FCR        0x02 << 3
#define IIR        0x02 << 3
#define LCR        0x03 << 3
#define MCR        0x04 << 3
#define LSR        0x05 << 3
#define MSR        0x06 << 3
#define SPR        0x07 << 3
#define TXLVL      0x08 << 3
#define RXLVL      0x09 << 3
#define DLAB       0x80 << 3
#define IODIR      0x0A << 3
#define IOSTATE    0x0B << 3
#define IOINTMSK   0x0C << 3
#define IOCTRL     0x0E << 3
#define EFCR       0x0F << 3

#define DLL        0x00 << 3
#define DLM        0x01 << 3
#define EFR        0x02 << 3
#define XON1       0x04 << 3  
#define XON2       0x05 << 3
#define XOFF1      0x06 << 3
#define XOFF2      0x07 << 3

// See Chapter 11 of datasheet
#define SPI_READ_MODE_FLAG 0x80

#define BAUD_RATE_DEFAULT 9600 // WiFly default baudrate

class UartBridge: public Stream {
public:
	const byte LCR_DIVISOR_LATCH_ENABLE = 1 << 7; // normal default = 0 latch disabled
	const byte LCR_BREAK_CONTROL = 1 << 6; // normal default = 0
	const byte LCR_SET_PARITY = 1 << 5; // normal default = 0 parity not forced
	const byte LCR_PARITY_TYPE_SELECT = 1 << 4; // no default
	const byte LCR_PARITY_ENABLE = 1 << 3; // normal default = 0 no parity
	const byte LCR_NUMBER_OF_STOP_BITS = 1 << 2; // 0 -- 1 stop bit, 1 -- 1.5 or 2 stop bits
	const byte LCR_WORD_LENGTH_BITS = 3; // 5 + LENGTH_BITS = 5,6,7,8

	const byte FCR_TXFIFO_RESET = 1 << 2;
	const byte FCR_RXFIFO_RESET = 1 << 1;
	const byte FCR_FIFO_ENABLE = 1;

private:
	byte cs_pin;

public:
	UartBridge(const byte pin = 10) {
		cs_pin = pin;
	}

	void begin(unsigned long baudrate = BAUD_RATE_DEFAULT);
	int available();
	int read();
	int peek();
	size_t write(byte value);
	size_t write(const char *str);
#if ENABLE_BULK_TRANSFERS
	void write(const uint8_t *buffer, size_t size);
#else
	using Print::write;
#endif
	void flush();

	// These are specific to the SPI UART
	void ioSetDirection(unsigned char bits);
	void ioSetState(unsigned char bits);

	void select();
	void deselect();

private:
	void writeRegister(byte registerAddress, byte data);
	byte readRegister(byte registerAddress);
	void init(unsigned long baudrate);
	void configureCommunication(byte databit = 8, byte stopbit = 1, boolean parity = false);
	void setBaudRate(unsigned long baudrate);
	boolean uartConnected();
};

#endif 

