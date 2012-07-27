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

#ifndef IMU_h
#define IMU_h

#include "WProgram.h"
#include <HardwareSerial.h>

// misc
#define d2r 0.01745329252
#define PACKET_LENGTH 32

// ADXRS150 mv per degree (my v1 has these gyros on all 3 boards)
// If you have a different one, set the scale value here
#define gyro_mVtheta 0.0125

// Scale factors to get volts from ADC
#define a2v_8bit 0.01953125
#define a2v_10bit 0.0048828125
#define a2v_12bit 0.001220703125

// Scale factors to get angles from ADC
#define a2deg_8bit a2v_8bit/gyro_mVtheta
#define a2rad_8bit a2v_8bit/gyro_mVtheta * d2r
#define a2deg_10bit a2v_10bit/gyro_mVtheta
#define a2rad_10bit a2v_10bit/gyro_mVtheta * d2r
#define a2deg_12bit a2v_12bit/gyro_mVtheta
#define a2rad_12bit a2v_12bit/gyro_mVtheta * d2r

class IMU {
	public:
		IMU(HardwareSerial *serialPort);
		void start();
		void stop();
		void setNulls();
		int packetReady();

		// filtered data
		float f_accel[3];
		float f_rate[3];

	private:
		void parsePacket();
		void filterData();

		// imu packet data
		uint8_t _imuBuffLen;
		unsigned char _imuBuffer[PACKET_LENGTH];
		int _startFlag;

		// serial point
		HardwareSerial *_serialPort;

		// raw data
		short _raw_accel[3][2];
		short _raw_accel_null[3][2];

		short _raw_rate[3];
		short _raw_rate_null[3];

		short _raw_vref[3];
		short _raw_vref_null[3];

		short _raw_temp[3];
		short _raw_temp_null[3];
		short _raw_vbatt;
	};

#endif