/*
 * Roomba500.cpp
 *
 *  Created on: 2012/02/12
 *      Author: sin
 */

#include <Roomba500.h>


void Roomba500::BRC_pulse(long d, long u) {
	digitalWrite(pin_brc, LOW);
	delay(d);
	digitalWrite(pin_brc, HIGH);
	delay(u);
}

void Roomba500::force_serial19200() {
	BRC_pulse();
	BRC_pulse();
	BRC_pulse();
}

void Roomba500::start() {
	port.write(highByte(START));
	delay(MODE_CHANGE_DELAY_MILLIS);
}

void Roomba500::safe() {
	port.write(highByte(CONTROL));
	delay(MODE_CHANGE_DELAY_MILLIS);
}

void Roomba500::power() {
	port.write(highByte(POWER));
}

void Roomba500::clean() {
	port.write(highByte(CLEAN));
}

void Roomba500::clean_max() {
	port.write(highByte(MAX));
}

void Roomba500::clean_spot() {
	port.write(highByte(SPOT));
}

void Roomba500::seekDock() {
	port.write(highByte(SEEKDOCK));
}

void Roomba500::baud(long speed) {
	port.write(BAUD);
	switch (speed) {
	case 9600:
		port.write(Baud9600);
		break;
	case 38400:
		port.write(Baud38400);
		break;
	case 57600:
		port.write(Baud57600);
		break;
	case 115200:
		port.write(Baud115200);
		break;
	case 19200:
	default:
		port.write(Baud19200);
		break;
	}
	delay(100);
}

void Roomba500::drive(int signed speed, word motion) {
	port.write(highByte(DRIVE));
	port.write(highByte(speed));
	port.write(lowByte(speed));
	port.write(highByte(motion));
	port.write(lowByte(motion));
}

int Roomba500::distance() {
	int tmp;
	getSensorPacket(Distance, (byte*) &tmp);
	return tmp;
}

int Roomba500::angle() {
	int tmp;
	getSensorPacket(Angle, (byte*) &tmp);
	return tmp;
}

byte Roomba500::chargingSources() {
	byte t;
	getSensorPacket(ChargingSourcesAvailabe, (byte*) &t);
	return t;
}

byte Roomba500::mode() {
	byte t;
	getSensorPacket(OIMODE, (byte*) &t);
	return t;
}

word Roomba500::wallSignal() {
	word buf;
	getSensorPacket(WallSignal, (byte*) &buf);
	return (buf << 8 | buf >> 8);
}

byte Roomba500::IRCharacterOmni() {
	byte t;
	getSensorPacket(InfraredCharacterOmni, (byte*) &t);
	return t;
}

void Roomba500::getIRCharacters(byte & omni, byte & left, byte & right) {
	byte pid[] = { highByte(InfraredCharacterOmni),
			highByte(InfraredCharacterLeft),
			highByte(InfraredCharacterRight) };
	byte t[3];
	getSensorList(pid, 3, (byte*) &t);
	omni = t[0];
	left = t[1];
	right = t[2];
	return;
}

byte Roomba500::lightBumper() {
	byte t;
	getSensorPacket(LightBumper, (byte*) &t);
	return t;
}

void Roomba500::getLightBumpSignals(word values[6]) {
	byte t[12];
	getSensorPacket(highlow(106, 12), t);
	for (int i = 0; i < 12; i += 2) {
		values[i >> 1] = highlow(t[i], t[i+1]);
	}
}

void Roomba500::getWallCliffSignals(word values[5]) {
	byte t[14];
	getSensorPacket(highlow(4,14), t);
	for (int i = 0; i < 10; i += 2) {
		values[i >> 1] = highlow(t[i], t[i+1]);
	}
}

void Roomba500::LEDs(byte bits, byte pwcolor, byte pwint) {
	port.write(highByte(LEDS));
	port.write(bits);
	port.write(pwcolor);
	port.write(pwint);
}

void Roomba500::setSong(const byte num, const char song[],const byte len) {
	if ( len < 1 )
		return;
	port.write(highByte(SONG));
	port.write(num % 5);
	port.write(min(len, 16));
	for(byte i = 0; i < min(len, 16); i++) {
		port.write(song[2*i]);
		port.write(song[2*i+1]);
	}
}

void Roomba500::playSong(const byte num) {
	port.write(highByte(PLAY));
	port.write(num % 5);
}

boolean Roomba500::songPlaying() {
	byte res[1];
	getSensorPacket(SONGPLAYING, res);
	return res[0] == 1;
}

void Roomba500::getSensorPacket(word packetInfo, byte buf[]) { //センサー読み取りコマンド
	port.write(highByte(SENSORS));
	port.write(highByte(packetInfo));
	long waiting = millis();
	for (int i = 0; i < lowByte(packetInfo) and millis() <= waiting + 100;
			) {
		if (port.available()) {
			buf[i++] = (byte) port.read();
			waiting = millis();
		}
	}
}

void Roomba500::getSensorList(byte packetids[], byte n, byte buf[]) {
	port.write(highByte(QUERYLIST));
	port.write(n);
	port.write(packetids, n);
	long waiting = millis();
	for (int i = 0; millis() <= waiting + 100;) {
		if (port.available()) {
			int c = port.read();
			buf[i++] = c;
			waiting = millis();
		}
	}
}

byte Roomba500::bumpsAndWheelDrops() {
	byte tmp;
	getSensorPacket(BumpsAndWheelDrops, &tmp);
	return tmp;
}

byte Roomba500::cliffsAndWalls() {
	byte tmp[10];
	getSensorPacket(highlow(1,10), tmp);
	return (tmp[1] << 4 | tmp[2] >> 1 | tmp[3] | tmp[4] << 1 | tmp[5] << 2
			| tmp[6] << 5);
}

byte Roomba500::chargingState() {
	byte t;
	getSensorPacket(ChargingState, &t);
	return t;
}

word Roomba500::voltage() {
	byte t[2];
	getSensorPacket(Voltage, t);
	return highlow(t[0], t[1]);
}

word Roomba500::rightEncoderCounts() {
	byte t[2];
	getSensorPacket(RightEncoderCounts, t);
	return highlow(t[0], t[1]);
}

word Roomba500::leftEncoderCounts() {
	byte t[2];
	getSensorPacket(LeftEncoderCounts, t);
	return highlow(t[0], t[1]);
}
