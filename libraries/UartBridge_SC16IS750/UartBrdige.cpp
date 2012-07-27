#include "UartBridge.h"
#if ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif
#include <SPI.h>

// See section 8.10 of the datasheet for definitions
// of bits in the Enhanced Features Register (EFR)
#define EFR_ENABLE_CTS 1 << 7
#define EFR_ENABLE_RTS 1 << 6
#define EFR_ENABLE_ENHANCED_FUNCTIONS 1 << 4


// See section 8.4 of the datasheet for definitions
// of bits in the Line Control Register (LCR)
#define LCR_ENABLE_DIVISOR_LATCH 1 << 7


// The original crystal frequency used on the board (~12MHz) didn't
// give a good range of baud rates so around July 2010 the crystal
// was replaced with a better frequency (~14MHz).
#ifndef USE_14_MHZ_CRYSTAL
#define USE_14_MHZ_CRYSTAL false // true (14MHz) , false (12 MHz)
#endif

#if USE_14_MHZ_CRYSTAL
#define XTAL_FREQUENCY 14745600UL // On-board crystal (New mid-2010 Version)
#else
#define XTAL_FREQUENCY 12288000UL // On-board crystal (Original Version)
#endif

// See datasheet section 7.8 for configuring the
// "Programmable baud rate generator"
#define PRESCALER 1 // Default prescaler after reset
#define BAUD_RATE_DIVISOR(baud) ((XTAL_FREQUENCY/PRESCALER)/(baud*16UL))



void UartBridge::begin(unsigned long baudrate /* default value */) {
  /*
        
   */
	//UartBridge::begin();
  init(baudrate);
}


void UartBridge::init(unsigned long baudrate) {
  /*
    
    Initialise the UART.
    
    If initialisation fails this method does not return.
        
   */
  // Initialize and test SC16IS750
  configureUart(baudrate);
  
  if(!uartConnected()){ 
    while(1) {
      // Lock up if we fail to initialise SPI UART bridge.
    }; 
  }
  
  // The SPI UART bridge is now successfully initialised.
}


void UartBridge::setBaudRate(unsigned long baudrate) {
	const byte LCR_DIVISOR_LATCH_ENABLE = 1<<7;
  /*
   */
  unsigned long divisor = BAUD_RATE_DIVISOR(baudrate);

  writeRegister(LCR, LCR_DIVISOR_LATCH_ENABLE); //LCR_ENABLE_DIVISOR_LATCH); // "Program baudrate"
  writeRegister(DLL, lowByte(divisor));
  writeRegister(DLM, highByte(divisor)); 
}

void UartBridge::configureCommunication(byte databit, byte stopbit, boolean parity) {
  /*
  
     Configure the settings of the UART.
  
   */
  // TODO: Improve with use of constants and calculations.
//  setBaudRate(baudrate);

	byte bits = 0; // same with the default
	bits |= LCR_WORD_LENGTH_BITS & (databit - 5);
	if ( stopbit != 1 ) {
		bits |= LCR_NUMBER_OF_STOP_BITS;
	}
	if ( parity ) {
		bits |= LCR_SET_PARITY | LCR_PARITY_TYPE_SELECT | LCR_PARITY_ENABLE;
	}
	writeRegister(LCR, bits);
	writeRegister(EFR, EFR_ENABLE_ENHANCED_FUNCTIONS); // enable enhanced registers
	writeRegister(FCR, FCR_TXFIFO_RESET | FCR_RXFIFO_RESET); // reset TXFIFO, reset RXFIFO
	// wait two clock
	writeRegister(FCR, FCR_FIFO_ENABLE); // enable FIFO mode
}


boolean UartBridge::uartConnected() {
  /*
  
     Check that UART is connected and operational.
  
   */
  // Perform read/write test to check if UART is working
  const char TEST_CHARACTER = 'H';
  
  writeRegister(SPR, TEST_CHARACTER);

  return (readRegister(SPR) == TEST_CHARACTER);
}


void UartBridge::writeRegister(byte registerAddress, byte data) {
  /*

    Write <data> byte to the SC16IS750 register <registerAddress>.

   */
  select();
  SPI.transfer(registerAddress);
  SPI.transfer(data);
  deselect();
}


byte UartBridge::readRegister(byte registerAddress) {
  /*
  
    Read byte from SC16IS750 register at <registerAddress>.
  
   */
  // Used in SPI read operations to flush slave's shift register
  const byte SPI_DUMMY_BYTE = 0xFF; 
  
  char result;

  select();
  SPI.transfer(SPI_READ_MODE_FLAG | registerAddress);
  result = SPI.transfer(SPI_DUMMY_BYTE);
  deselect();
  return result;  
}


int UartBridge::available() {
  /*
    Get the number of bytes (characters) available for reading.

    This is data that's already arrived and stored in the receive
    buffer (which holds 64 bytes).
   */
  // This alternative just checks if there's data but doesn't
  // return how many characters are in the buffer:
  //    readRegister(LSR) & 0x01
  return readRegister(RXLVL);
}


int UartBridge::read() {
  /*
    Read byte from UART.
    Returns byte read or or -1 if no data available.
    Acts in the same manner as 'Serial.read()'.
   */
  if (!available()) {
    return -1;
  }
  
  return readRegister(RHR);
}


size_t UartBridge::write(byte value) {
  /*
  
    Write byte to UART.
 
   */
  while (readRegister(TXLVL) == 0) {
    // Wait for space in TX buffer
  };
  writeRegister(THR, value); 
  return 1;
}


size_t UartBridge::write(const char *str) {
  /*
  
    Write string to UART.
 
   */
  int s = write((const char *) str); //, strlen(str));
  while (readRegister(TXLVL) < 64) {
    // Wait for empty TX buffer (slow)
    // (But apparently still not slow enough to ensure delivery.)
  };
  return s;
}

#if ENABLE_BULK_TRANSFERS
void UartBridge::write(const uint8_t *buffer, size_t size) {
  /*
  
    Write buffer to UART.
 
   */
  select();
  transfer(THR); // TODO: Change this when we modify register addresses? (Even though it's 0x00.) 

  while(size > 16) {
    transfer_bulk(buffer, 16);
    size -= 16;
    buffer += 16;
  }
  transfer_bulk(buffer, size);

  deselect();
}
#endif

void UartBridge::flush() {
  /*
  
    Flush characters from SC16IS750 receive buffer.
  
   */
  // Note: This may not be the most appropriate flush approach.
  //       It might be better to just flush the UART's buffer
  //       rather than the buffer of the connected device
  //       which is essentially what this does.
  while(available() > 0) {
    read();
  }
}


void UartBridge::ioSetDirection(unsigned char bits) {
  /*
   */
  writeRegister(IODIR, bits);
}


void UartBridge::ioSetState(unsigned char bits) {
  /*
   */
  writeRegister(IOSTATE, bits);
}


void UartBridge::select() {
   SPI.setClockDivider(SPI_CLOCK_DIV8);
   SPI.setDataMode(SPI_MODE0);
   SPI.setBitOrder(MSBFIRST);
   if ( cs_pin != 0xff )
      digitalWrite(cs_pin, LOW);
}
void UartBridge::deselect() {
   if ( cs_pin != 0xff )
      digitalWrite(cs_pin, HIGH);
}
