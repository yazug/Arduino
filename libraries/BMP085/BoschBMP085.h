/*
 *  BMP085.h
 *  
 *
 *  Created by Sin on 10/04/25.
 *  Copyright 2010 Kyutech. All rights reserved.
 *
 */

class BoschBMP085 {
//	static int pressure_wait_ms[4] = { 5, 8, 14, 26 };
	static const int i2cAddress = 0x77;

	byte oss;
	//just taken from the BMP085 datasheet
	int ac1;
	int ac2; 
	int ac3; 
	unsigned int ac4;
	unsigned int ac5;
	unsigned int ac6;
	int b1; 
	int b2;
	int mb;
	int mc;
	int md;

	int temp;
	long press;
public:
	BoschBMP085();
	
	void calibrate();
	word read_register_word(byte r);
	void write_register(byte r, byte v);
	void sensor_read();
	int read_ut();
	unsigned long read_up();
	
	int temperature() {
		return temp;
	}
	long pressure() {
		return press;
	}
};

