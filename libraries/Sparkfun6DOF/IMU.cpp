/*
 * IMU.cpp
 *
 * Designed for the Sparkfun 6DOF v1 IMU
 * Note: setNulls uses my default values. You may need to tweak them
 *
 * (c) 2005-9 Filipe Varela

 * IMU is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * IMU is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with IMU; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "WProgram.h"
#include "IMU.h"

// Not really a ntohs because we WANT to flip, not determine if we NEED to flip
#define ntohs(n) ((((short)(n)) & 0xff00) >> 8 | (((short)(n)) & 0xff) << 8)

IMU::IMU(HardwareSerial *serialPort) {
	_serialPort = serialPort;
	_startFlag = 0;
	_imuBuffLen = 0;
	memset(_imuBuffer, 0, sizeof(_imuBuffer));
}

void IMU::start() {
	this->stop();
	_serialPort->write(7); // ctrl-g
}

void IMU::stop() {
	_serialPort->write(19); // ctrl-s
}

void IMU::setNulls() {
	// when called, copies current values to offset
	_raw_accel_null[0][0] = _raw_accel[0][0];
	_raw_accel_null[0][1] = _raw_accel[0][1];

	_raw_accel_null[1][0] = _raw_accel[1][0];
	_raw_accel_null[1][1] = _raw_accel[1][1];

	_raw_accel_null[2][0] = _raw_accel[2][0];
	_raw_accel_null[2][1] = _raw_accel[2][1];

	_raw_rate_null[0] = _raw_rate[0];
	_raw_rate_null[1] = _raw_rate[1];
	_raw_rate_null[2] = _raw_rate[2];

	_raw_temp_null[0] = _raw_temp[0];
	_raw_temp_null[1] = _raw_temp[1];
	_raw_temp_null[2] = _raw_temp[2];

	_raw_vref_null[0] = _raw_vref[0];
	_raw_vref_null[1] = _raw_vref[1];
	_raw_vref_null[2] = _raw_vref[2];

	//return;

	_raw_rate_null[0] = 480;
	_raw_rate_null[1] = 512;
	_raw_rate_null[2] = 480;

	_raw_accel_null[0][0] = 510;
	_raw_accel_null[0][1] = 510;

	_raw_accel_null[1][0] = 510;
	_raw_accel_null[1][1] = 510;

	_raw_accel_null[2][0] = 504;
	_raw_accel_null[2][1] = 504;
}

int IMU::packetReady() {
	int i;
	
	// test for packet + header + footer
	if (_serialPort->available() < PACKET_LENGTH+2)
		return 0;
	
	if (_serialPort->read() != 'A')
		return 0;
	
	for (i=0; i<PACKET_LENGTH; i++)
		_imuBuffer[i] = _serialPort->read();
	
	if (_serialPort->read() != 'Z')
		return 0;
	
	this->parsePacket();
	this->filterData();
	
	return 1;
}

void IMU::parsePacket() {
	// skip 'A'
	unsigned char *ptr = _imuBuffer;

	// pitch board
	ptr += 0; memcpy(&_raw_rate[1], ptr, 2); // gyro rateout
	ptr += 2; memcpy(&_raw_vref[1], ptr, 2); // gyro vref (2.5v)
	ptr += 2; memcpy(&_raw_temp[1], ptr, 2); // gyro temp
	ptr += 2; memcpy(&_raw_accel[1][0], ptr, 2); // accel y (real axis = -y)
	ptr += 2; memcpy(&_raw_accel[1][1], ptr, 2); // accel x (real axis = z)

	// roll board
	ptr += 2; memcpy(&_raw_rate[0], ptr, 2); // gyro rateout
	ptr += 2; memcpy(&_raw_vref[0], ptr, 2); // gyro vref (2.5v)
	ptr += 2; memcpy(&_raw_temp[0], ptr, 2); // gyro temp
	ptr += 2; memcpy(&_raw_accel[0][0], ptr, 2); // accel y (real axis = x)
	ptr += 2; memcpy(&_raw_accel[0][1], ptr, 2); // accel x (real axis = z)

	// yaw board
	ptr += 2; memcpy(&_raw_rate[2], ptr, 2); // gyro rateout
	ptr += 2; memcpy(&_raw_vref[2], ptr, 2); // gyro vref (2.5v)
	ptr += 2; memcpy(&_raw_temp[2], ptr, 2); // gyro temp
	ptr += 2; memcpy(&_raw_accel[2][0], ptr, 2); // accel y (real axis = y)
	ptr += 2; memcpy(&_raw_accel[2][1], ptr, 2); // accel x (real axis = x)

	// battery voltage
	ptr += 2; memcpy(&_raw_vbatt, ptr, 2);

	// now clean them up. the imu is big endian and the avr is little
	_raw_rate[0] = ntohs(_raw_rate[0]);
	_raw_rate[1] = ntohs(_raw_rate[1]);
	_raw_rate[2] = ntohs(_raw_rate[2]);

	_raw_accel[0][0] = ntohs(_raw_accel[0][0]);
	_raw_accel[0][1] = ntohs(_raw_accel[0][1]);
	_raw_accel[1][0] = ntohs(_raw_accel[1][0]);
	_raw_accel[1][1] = ntohs(_raw_accel[1][1]);
	_raw_accel[2][0] = ntohs(_raw_accel[2][0]);
	_raw_accel[2][1] = ntohs(_raw_accel[2][1]);

	_raw_vref[0] = ntohs(_raw_vref[0]);
	_raw_vref[1] = ntohs(_raw_vref[1]);
	_raw_vref[2] = ntohs(_raw_vref[2]);

	_raw_temp[0] = ntohs(_raw_temp[0]);
	_raw_temp[1] = ntohs(_raw_temp[1]);
	_raw_temp[2] = ntohs(_raw_temp[2]);
}

void IMU::filterData() {
	// parse accelerometer values
	// pitch board
	_raw_accel[0][0] = _raw_accel[0][0] - _raw_accel_null[0][0];
	_raw_accel[0][1] = _raw_accel[0][1] - _raw_accel_null[0][1];

	// roll board
	_raw_accel[1][0] = _raw_accel[1][0] - _raw_accel_null[1][0];
	_raw_accel[1][1] = _raw_accel[1][1] - _raw_accel_null[1][1];

	// yaw board
	_raw_accel[2][0] = _raw_accel[2][0] - _raw_accel_null[2][0];
	_raw_accel[2][1] = _raw_accel[2][1] - _raw_accel_null[2][1];

	// X acceleration: roll board->y value + yaw board->x value
	f_accel[0] = ((float)_raw_accel[0][0] + (float)_raw_accel[2][1])/2.0;

	// Y acceleration: pitch board->-y value + yaw board->y value
	f_accel[1] = ((float)-_raw_accel[1][0] + (float)_raw_accel[2][0])/2.0;

	// Z acceleration: pitch board->x value + roll board->x value 
	f_accel[2] = ((float)_raw_accel[0][1] + (float)_raw_accel[1][1])/2.0;

	// parse gyro values
	f_rate[0] = (_raw_rate[0] - _raw_rate_null[0]) * a2rad_10bit;
  f_rate[1] = (_raw_rate[1] - _raw_rate_null[1]) * a2rad_10bit;
  f_rate[2] = (_raw_rate[2] - _raw_rate_null[2]) * a2rad_10bit;
}
