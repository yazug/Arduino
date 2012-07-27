/**
 * SCP1000 Library
 *
 *
 * Manages communication with SCP1000 barometric pressure & temperature sensor
 *
 */
#include "WProgram.h"
#include "SPI.h"
#include "SCP1000D01.h"



SCP1000::SCP1000(const byte selectPin, const byte DRDYPin)
{
	// User chosen Slave Select Pin
	_selectPin = selectPin;
	_DRDYPin = DRDYPin;
}

/* ================ Public methods ================ */

void SCP1000::init()
{
	// Set Pin directions
	pinMode(DataOutPin, OUTPUT);
	pinMode(DataInPin, INPUT);
	pinMode(SPIClockPin,OUTPUT);
	pinMode(_selectPin,OUTPUT);
	digitalWrite(_selectPin,HIGH); //disable device  
	if (_DRDYPin != -1 )
		pinMode(_DRDYPin, INPUT);
	
	// Set SPI control register
	// SPIE = 0	no interupt
	// SPE = 1	SPI enabled
	// DORD = 0	(MSB first)
	// MSTR = 1 (master)
	// CPOL = 0 (clock idle when low)
	// CPHA = 0 (samples MOSI on rising edge)
	// SPR1 = 1 & SPR0 = 1 (125kHz)
//	SPCR = 0b01010011;
//	SPSR = 0b00000000;
	
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV128);
	SPI.setDataMode(SPI_MODE0);
	delay(100);		// Allow SCP1000 to complete initialization
	run();
	
}

/*
 * Fetch values from the sensor
 */
/*
void SCP1000::read()
{
	readTemperature();
	readPressure();
}
*/
/*
 * Send a soft reset
 */
void SCP1000::reset() 
{
	write_register(RSTR, 0x01);
	delay(100);		// allow time for reset to complete
}

/*
 * Set sensor to standby mode
 * Stops all sensor operations
 */
void SCP1000::standby()
{
	write_register(OPREG, StandbyMode);
}

/*
 * Set sensor to run mode
 */
void SCP1000::run()
{
	// Set SCP1000 Mode -- Hi resolution, continuous readings
	write_register(OPREG, HiResMode);
}

/* ================ Private methods ================ */

/*
 * Reads current pressure value
 */
long SCP1000::pressure()
{
	unsigned long pressure;
	
	// Pressure value is in 19-bit unsigned format
	// Value = Pa * 4
	pressure = read_register(PRESSURE, 1);  // Read MSB
	pressure &= 0b00000111;					// mask unused bits
	pressure <<= 16;						// shift into upper word
	
	pressure  |= read_register(PRESSURE_LSB, 2); // read low word
	
	// Convert to real pressure in hPa
	return pressure>>2;
	
}


/*
 * Reads the current temperature value
 */
long SCP1000::temperature()
{
	unsigned long temp_in;
	
	// Temperature word is 14-bit signed int = (DegC * 20)
	temp_in = read_register(TEMP, 2);
	
	// Shift sign bit (bit 13) to proper position for signed int (bit 15)
	// This is equivalent to multiplying by 4, so now temp_in = DegC * 80
	temp_in <<= 2;
	temp_in *= 5;
	return (temp_in>>2); // Convert to real DegC
}

/*
 * Read a register from the device; 1 or 2 bytes
 */
unsigned int SCP1000::read_register(byte register_name, byte numBytes)
{
	unsigned int in_word;
	
	// SCP1000 registers are either 1 or 2 bytes long
	numBytes = (numBytes > 2) ? 2 : numBytes;  // ensure # of bytes is 0..2
	
	// SCP1000 command format is 6-bit address, 1-bit R/W, and 1-bit "0"
	register_name <<= 2;		// Shift register address to upper bits
	register_name &= 0b11111100;	//Read command
	
	digitalWrite(_selectPin,LOW);	// Select SPI Device
	SPI.transfer(register_name);	// Send register address to device
	
	for(; numBytes > 0; --numBytes) {
		in_word <<= 8;					// move existing bits up by one byte
		in_word |= SPI.transfer(0x00);	// add next byte
	}
	
	digitalWrite(_selectPin,HIGH);	// End Communiction
	
	return(in_word);
}

/*
 * Write single byte to a device register
 */
void SCP1000::write_register(byte register_name, byte data)
{
	//SCP1000 command format is 6-bit address, 1-bit R/W, and 1-bit "0"
	register_name <<= 2;			// Shift register address to upper bits
	register_name |= 0b00000010;		// Write command
	
	digitalWrite(_selectPin,LOW);	//Select SPI device
	SPI.transfer(register_name);	//Send register location
	SPI.transfer(data);				//Send value to record into register
	digitalWrite(_selectPin,HIGH);	// End Communication
	
}
