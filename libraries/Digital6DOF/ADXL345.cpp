/*
	ADXL345 Library
	
	This libary contains functions to interact with the ADXL345 Triple Axis Digital Accelerometer from Analog Devices written for the ATmega168
	
	created 20 Aug 2009
	by Ryan Owens
	http://www.sparkfun.com
	
 
*/
#include <Wire.h>
#include "ADXL345.h"
#include <stdlib.h>
#include <stdio.h>
//#include "main.h"
/*
extern "C"{
#include "twi.h"
}
*/
/*
#define GLOBALOBJECT

//Create a class instance to use in the main program
ADXL345 accelerometer(ADXL_ADDR);
char status=0;
*/

//Constructor defines the I2C address
ADXL345::ADXL345(char i2c_address)
{
	_i2c_address = i2c_address;
	
}

//Initialize the I2C communication and put the accelerometer in Measure mode
char ADXL345::begin(void)
{
//	twiInit(50000);	//Set the I2C clock speed to 50 kHz
	
	//Put the accelerometer in MEASURE mode
	write(POWER_CTL, MEASURE);
	
	//Set the Range to +/- 4G
	return write(DATA_FORMAT, RANGE_0);
	
	//default ADXL345 rate is 100 Hz. Perfect!
}

void ADXL345::powerDown(void)
{

}

//Read a register value from the ADXL345
//pre: register_addr is the register address to read
//	   value is a pointer to an integer
//post: value contains the value of the register that was read
//returns: 1-Success
//		   TWSR-Failure (Check out twi.h for TWSR error codes)
//usage: status = accelerometer.read(DEVID, &value); //value is created as an 'int' in main.cpp
char ADXL345::read(char register_addr, char * value){
//	twiReset();
//	return twiReceive(_i2c_address, register_addr, value);
	Wire.beginTransmission(_i2c_address);
	Wire.write(register_addr);
	Wire.endTransmission();
	char t = Wire.requestFrom(_i2c_address,1);
	*value = Wire.read();
	return t;
}

char ADXL345::read(char register_addr, char * value, int num){
//	twiReset();
//	return twiReceive(_i2c_address, register_addr, value);
	Wire.beginTransmission(_i2c_address);
	Wire.write(register_addr);
	Wire.endTransmission();
	char t = Wire.requestFrom(_i2c_address, num);
	for ( ; num > 0; num--) {
		*value++ = Wire.read();
	}
	return t;
}

//Write a value to a register
//pre: register_addre is the register to write to
//	   value is the value to place in the register
//returns: 1-Success
//		   TWSR- Failure
//usage status=accelerometer.write(register_addr, value);
char ADXL345::write(char register_addr, char value){
//	twiReset();
//	return twiTransmit(_i2c_address, register_addr, value);
	Wire.beginTransmission(_i2c_address);
	Wire.write(register_addr); //Wire.send(addr); // reset register pointer
	Wire.write(value);
	return Wire.endTransmission();
}

//Reads the x,y and z registers and stores the contents into x,y and z variables
//returns 1
//usage: accelerometer.update();
//Note: update must be called before using the getX, getY or getZ functions in order
//      to obtain the most recent values from the accelerometer
char ADXL345::update(void)
{
/*
	char temp=0;
	read(DATAX0, &value);
	temp=value;
	read(DATAX1, &value);
	x = (value<<8)|temp;
	
	read(DATAY0, &value);
	temp=value;
	read(DATAY1, &value);
	y = (value<<8)|temp;

	read(DATAZ0, &value);
	temp=value;
	read(DATAZ1, &value);
	z = (value<<8)|temp;	
*/
	//int temp[3];
	read(DATAX0, (char*)values, 6);
//	x = temp[0];
//	y = temp[1];
//	z = temp[2];

	return 1;
}

/*
get functions return the g value of the specified axis
The conversion is based on a +/-4G range.
If range is changed, make sure to update the scaling in the get functions
usage: printf("Xg = %1.3fg", (double)accelerometer.getX()
*/
float ADXL345::getX(void)
{
	return (float)values[0]*0.0078;
//	return xg;
}

float ADXL345::getY(void)
{
	return (float)values[1]*0.0078;
//	return yg;
}

float ADXL345::getZ(void)
{
	return (float)values[2]*0.0078;
//	return zg;
}
