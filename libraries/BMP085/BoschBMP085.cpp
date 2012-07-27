/*
 *  BMP085.cpp
 *  
 *
 *  Created by Sin on 10/04/25.
 *  Copyright 2010 Kyutech. All rights reserved.
 *
 */

#include <WProgram.h>
#include <Wire.h>
#include "BoschBMP085.h"

BoschBMP085::BoschBMP085() {
	oss = 3;
}

void  BoschBMP085::calibrate() {
//  Serial.println("Reading Calibration Data");
  ac1 = read_register_word(0xAA);
//  Serial.print("AC1: ");
//  Serial.println(ac1,DEC);
  ac2 = read_register_word(0xAC);
//  Serial.print("AC2: ");
//  Serial.println(ac2,DEC);
  ac3 = read_register_word(0xAE);
//  Serial.print("AC3: ");
//  Serial.println(ac3,DEC);
  ac4 = read_register_word(0xB0);
//  Serial.print("AC4: ");
//  Serial.println(ac4,DEC);
  ac5 = read_register_word(0xB2);
//  Serial.print("AC5: ");
//  Serial.println(ac5,DEC);
  ac6 = read_register_word(0xB4);
//  Serial.print("AC6: ");
//  Serial.println(ac6,DEC);
  b1 = read_register_word(0xB6);
//  Serial.print("B1: ");
//  Serial.println(b1,DEC);
  b2 = read_register_word(0xB8);
//  Serial.print("B2: ");
//  Serial.println(b1,DEC);
  mb = read_register_word(0xBA);
//  Serial.print("MB: ");
//  Serial.println(mb,DEC);
  mc = read_register_word(0xBC);
//  Serial.print("MC: ");
//  Serial.println(mc,DEC);
  md = read_register_word(0xBE);
//  Serial.print("MD: ");
//  Serial.println(md,DEC);
}

word BoschBMP085::read_register_word(byte r)
{
	word msb;
	byte lsb;
	
	Wire.beginTransmission(i2cAddress);
	Wire.send(r);  // register to read
	Wire.endTransmission();
	
	Wire.requestFrom(i2cAddress, 2); // read a byte
	while(!Wire.available()) {
		// waiting
	}
	msb = Wire.receive();
	while(!Wire.available()) {
		// waiting
	}
	lsb = Wire.receive();
	return (msb<<8) | lsb;
}


void BoschBMP085::write_register(byte r, byte v)
{
	Wire.beginTransmission(i2cAddress);
	Wire.send(r);
	Wire.send(v);
	Wire.endTransmission();
}



void BoschBMP085::sensor_read() {
	int  ut= read_ut();
	unsigned long up = read_up();
	up = read_up();
	long x1, x2, x3, b3, b5, b6, p;
	unsigned long b4, b7;
	
	//calculate the temperature
	x1 = (((long)ut - ac6) * ac5) >> 15;
	x2 = ((long) mc << 11) / (x1 + md);
	b5 = x1 + x2;
	temp = (b5 + 8) >> 4;
		
	b6 = b5 - 4000;
	x1 = (b2 * ((b6 * b6) >> 12)) >> 11;
	x2 = (ac2 * b6) >> 11;
	x3 = x1 + x2;
	b3 = ((((long)ac1 * 4 + x3)<<oss) + 2)/4;
	x1 = (ac3 * b6) >> 13;
	x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (unsigned long) (x3 + 0x8000L)) >> 15;
	b7 = ((unsigned long) up - b3) * (50000 >> oss);
	p = b7 < 0x80000000 ? (b7 *2)/b4 : (b7 / b4) * 2;
	x1 = (p >> 8); x1 *= x1;
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
//	Serial.print("; p=");
//	Serial.println(p);
	press = p + ((x1 + x2 + 3791) >> 4);
	
}



int BoschBMP085::read_ut() {
	write_register(0xf4,0x2e);
	delay(5); //longer than 4.5 ms
	return read_register_word(0xf6);
}


unsigned long BoschBMP085::read_up() {
	static byte pressure_wait_ms[4] = { 5, 8, 14, 26 };

	write_register(0xf4,0x34+(oss<<6));
	delay(2*pressure_wait_ms[oss]);
	
	long msb;
	byte lsb, xlsb;
	Wire.beginTransmission(i2cAddress);
	Wire.send(0xf6);  // register to read
	Wire.endTransmission();
	
	Wire.requestFrom(i2cAddress, 3); // read a byte
	while(!Wire.available()) {
		// waiting
	}
	msb = Wire.receive();
	while(!Wire.available()) {
		// waiting
	}
	lsb = Wire.receive();
	while(!Wire.available()) {
		// waiting
	}
	xlsb = Wire.receive();
//	Serial.print("raw p: ");
//	Serial.println(((msb<<16) | (lsb<<8) | xlsb) >> (8 - oss), DEC);
//	Serial.print(" [");
//	Serial.print(((msb<<16) | (lsb<<8) | xlsb) >> (8 - oss), HEX);
//	Serial.println(" ]");
	return ((msb<<16) | (lsb<<8) | xlsb) >>(8-oss);
}
