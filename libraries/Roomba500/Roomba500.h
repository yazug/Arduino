/*
 * Roomba.h
 *
 *  Created on: 2012/02/02
 *      Author: sin
 */

#ifndef ROOMBA500_H_
#define ROOMBA500_H_

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define highlow(x,y)   ( ((word)(x))<<8 | (y) )

class Roomba500 {

private:
	// variables
	Stream & port;
	unsigned int pin_brc;
	enum InterfaceType {
		HARDWARESERIAL, SOFTWARESERIAL
	} serialType;

	static const int MODE_CHANGE_DELAY_MILLIS = 100;

	void BRC_pulse(long d = 50, long u = 250);
	inline void wakeup_pulse() {
		BRC_pulse(500, 2000);
	}
	void force_serial19200();

public:

	Roomba500(HardwareSerial & ser, int dd) :
			port(ser) {
		pin_brc = dd;
		serialType = HARDWARESERIAL;
	}

#ifdef SoftwareSerial_h
	Roomba500(SoftwareSerial & ser, int dd) : port(ser) {
		pin_brc = dd;
		serialType = SOFTWARESERIAL;
	}
#endif

	void begin(long baud = 57600) {
		pinMode(pin_brc, OUTPUT);
		digitalWrite(pin_brc, HIGH);
		wakeup_pulse();
		if (baud == 19200)
			force_serial19200();
		//
		switch (serialType) {
		case HARDWARESERIAL:
			((HardwareSerial &) port).begin(baud);
			break;
#ifdef SoftwareSerial_h
		case SOFTWARESERIAL:
			((SoftwareSerial &)port).begin(baud);
			break;
#endif
		}
		if (baud != 57600 and baud != 19200)
			this->baud(baud);
	}

	void send(byte code) {
		port.write(code);
	}

	void start();
	void safe();
	void power();
	void clean();
	void clean_max();
	void clean_spot();
	void seekDock();
	void baud(long speed);
	void drive(int signed speed, word motion);

	inline void goForward(int speed = 300) {
		drive(speed, Drive_Straight);
	}

	inline void goSlowly(int speed = 150) {
		drive(speed, Drive_Straight);
	}

	inline void goBackward(int speed = -200) {
		drive(-speed, Drive_Straight);
	}

	inline void spinLeft(int speed = 100) {
		drive(speed, Drive_InPlaceCounterClockwise); // 0x0001 == spin left
	}

	inline void spinRight(int speed = 100) {
		drive(speed, Drive_InPlaceClockwise); // 0xffff == -1 == spin right
	}

	inline void turnLeft(int speed = 200, int radius = 128) {
		drive(speed, radius); //
	}

	inline void turnRight(int speed = 200, int radius = 128) {
		drive(speed, (word) -radius); //
	}

	inline void stop() {
		drive(0, Drive_Straight); //
	}

	void LEDs(byte bits, byte pwcolor, byte pwint);
	void setSong(const byte snum, const char song[], const byte slength);
	void playSong(const byte songNumber);
	boolean songPlaying();
	void getSensorPacket(word packetInfo, byte buf[]);
	void getSensorList(byte packetids[], byte n, byte buf[]);

	byte bumpsAndWheelDrops();

	inline boolean bumpRight(const byte bw) {
		return (bw & BumpRight) != 0;
	}
	inline boolean bumpLeft(const byte bw) {
		return (bw & BumpLeft) != 0;
	}
	inline boolean wdropRight(const byte bw) {
		return (bw & WheelDropRight) != 0;
	}
	inline boolean wdropLeft(const byte bw) {
		return (bw & WheelDropLeft) != 0;
	}

	byte cliffsAndWalls();

	inline static boolean cliffLeft(const byte cw) {
		return (cw & CliffLeftBit) != 0;
	}
	inline static boolean cliffFrontLeft(const byte cw) {
		return (cw & CliffFrontLeftBit) != 0;
	}
	inline static boolean cliffFrontRight(const byte cw) {
		return (cw & CliffFrontRightBit) != 0;
	}
	inline static boolean cliffRight(const byte cw) {
		return (cw & CliffRightBit) != 0;
	}
	inline static boolean wall(const byte cw) {
		return (cw & WallBit) != 0;
	}
	inline static boolean virtualWall(const byte cw) {
		return (cw & VirtualWallBit) != 0;
	}

	int distance();
	int angle();
	byte chargingSources();
	boolean charging_HomeBase() { return (chargingSources() & ChargingSources_HomeBase) != 0; }
	boolean charging_InternalCharger() { return (chargingSources() & ChargingSources_InternalCharger) != 0; }
	byte mode();
	word wallSignal();

	byte IRCharacterOmni();
	void getIRCharacters(byte &omni, byte &left, byte &right);
	byte lightBumper();

	boolean lightBumperLeft(const byte b) {
		return (b & LightBumperLeft) != 0;
	}
	boolean lightBumperFrontLeft(const byte b) {
		return (b & LightBumperFrontLeft) != 0;
	}
	boolean lightBumperCenterLeft(byte b) {
		return (b & LightBumperCenterLeft) != 0;
	}
	boolean lightBumperCenterRight(byte b) {
		return (b & LightBumperCenterRight) != 0;
	}
	boolean lightBumperFrontRight(byte b) {
		return (b & LightBumperFrontRight) != 0;
	}
	boolean lightBumperRight(byte b) {
		return (b & LightBumperRight) != 0;
	}

	void getLightBumpSignals(word values[6]);
	void getWallCliffSignals(word values[5]);
	byte chargingState();
	word voltage();
	word rightEncoderCounts();
	word leftEncoderCounts();

// constant defs
public:
	static const word BumpsAndWheelDrops = 0x0701;
	static const byte BumpRight = 1 << 0, BumpLeft = 1 << 1, WheelDropRight = 1
			<< 2, WheelDropLeft = 1 << 3;
	static const word CliffLeft = 0x0901;
	static const word CliffFrontLeft = 0x0a01;
	static const word CliffFrontRight = 0x0b01;
	static const word CliffRight = 0x0c01;
	static const word Wall = 0x0801;
	static const word VirtualWall = 0x0d01;
	static const byte CliffLeftBit = 1 << 0, CliffFrontLeftBit = 1 << 1,
			CliffFrontRightBit = 1 << 2, CliffRightBit = 1 << 3, WallBit = 1
					<< 4, VirtualWallBit = 1 << 5;
	static const word WallSignal = highlow(27, 2);
	static const word CliffLeftSignal = highlow(28, 2);
	static const word CliffFrontLeftSignal = highlow(29, 2);
	static const word CliffFrontRightSignal = highlow(30, 2);
	static const word CliffRightSignal = highlow(31, 2);

	static const word GroupPacketID0 = highlow(0,26);
	static const word GroupPacketID1 = highlow(1,10);

	static const word WheelOverCurrents = 0x0e01;
	static const byte SideBrush = 1 << 0, MainBrush = 1 << 2, RightWheel = 1
			<< 3, LeftWheel = 1 << 4;
	static const word DirtDetect = 0x0f01;

	static const word InfraredCharacterOmni = 0x1101;
	static const word InfraredCharacterLeft = 0x3401;
	static const word InfraredCharacterRight = 0x3501;

	static const word Distance = highlow(19, 2);
	static const word Angle = highlow(20, 2);
	static const word ChargingState = highlow(21,1);
	static const word Voltage = highlow(22,2);
	static const word Current = highlow(23,2);

	static const word ChargingSourcesAvailabe = highlow(34, 1);
	static const byte ChargingSources_InternalCharger = 1 << 0,
			ChargingSources_HomeBase = 1 << 1;
	static const word OIMODE = highlow(35, 1);
	static const word RightEncoderCounts = highlow(43, 2);
	static const word LeftEncoderCounts = highlow(44, 2);

	static const word LightBumper = highlow(45, 1);
	static const byte LightBumperLeft = 1 << 0, LightBumperFrontLeft = 1 << 1,
			LightBumperCenterLeft = 1 << 2, LightBumperCenterRight = 1 << 3,
			LightBumperFrontRight = 1 << 4, LightBumperRight = 1 << 5;
	static const word LightBumpLeftSignal = highlow(46, 2);
	static const word LightBumpFrontLeftSignal = highlow(47, 2);
	static const word LightBumpCenterLeftSignal = highlow(48, 2);
	static const word LightBumpCenterRightSignal = highlow(49, 2);
	static const word LightBumpFrontRightSignal = highlow(50, 2);
	static const word LightBumpRightSignal = highlow(51, 2);

	static const word START = (word) 128 << 8 | 0;
	static const word BAUD = (word) 129 << 8 | 1;
	static const word CONTROL = (word) 130 << 8 | 0;
	static const word FULL = (word) 132 << 8 | 0;
	static const word POWER = (word) 133 << 8 | 0;
	static const word SPOT = (word) 134 << 8 | 0;
	static const word CLEAN = (word) 135 << 8 | 0;
	static const word MAX = (word) 136 << 8 | 0;
	static const word DRIVE = highlow(137, 4);

	static const word SEEKDOCK = highlow(143, 0);

	static const word DRIVE_DIRECT = highlow(145, 4);
	static const word SCHEDULE = (word) 167 << 8 | 0;

	static const word SET_DAY_TIME = highlow(168, 3);
	static const word LEDS = highlow(139, 3);
	static const byte LEDDirtDetect = 1 << 0, LEDMax = 1 << 1, LEDClean = 1
			<< 2, LEDSpot = 1 << 3, LEDStatus = 1 << 4;
	static const word SONG = highlow(140, 2);
	static const word PLAY = highlow(141, 1);
	static const word SONGPLAYING = highlow(37, 1);
	static const word SENSORS = highlow(142, 1);
	static const word QUERYLIST = highlow(149, 1);
	//
	enum Drive {
		Drive_Straight = 0x8000,
		Drive_InPlaceClockwise = 0xFFFF,
		Drive_InPlaceCounterClockwise = 0x0001
	};

	enum Baud {
		Baud300 = 0,
		Baud600 = 1,
		Baud1200 = 2,
		Baud2400 = 3,
		Baud4800 = 4,
		Baud9600 = 5,
		Baud14400 = 6,
		Baud19200 = 7,
		Baud28800 = 8,
		Baud38400 = 9,
		Baud57600 = 10,
		Baud115200 = 11
	};

	enum IR_Character {
		IR_Left = 129,
		IR_Forward = 130,
		IR_Right = 131,
		IR_Spot = 132,
		IR_Max = 133,
		IR_Small = 134,
		IR_Medium = 135,
		IR_Clean = 136,
		IR_Stop = 137,
		IR_Power = 138,
		IR_ArcLeft = 139,
		IR_ArcRight = 140,
		IR_Stop_2 = 141,
		//
		IR_ForceField = 161,
		IR_VirtualWall = 162,
		IR_GreenBuoy = 164,
		IR_GreenBuoyForceField = 165,
		IR_RedBuoy = 168,
		IR_RedBuoyForceField = 169,
		IR_RedBuoyGreenBuoy = 172,
		IR_RedBuoyGreenBuoyForceField = 173
	};

	enum ChargingState {
		NotCharging = 0,
		ReconditioningCharging,
		FullCharging,
		TrickleCharging,
		Waiting,
		ChargingFaultCondition
	};

	enum OIMode {
		Mode_Off = 0, Mode_Passive, Mode_Safe, Mode_Full
	};

};

#endif /* ROOMBA500_H_ */
