/*
 * PushButton.h
 *
 *  Created on: 2012/01/02
 *      Author: sin
 */

#ifndef PUSHBUTTON_H_
#define PUSHBUTTON_H_

#include <Arduino.h>

class PushButton {
	static const long defaultThreshold = 50;

	int pin;
	boolean status;
	boolean lastReading;
	long bounced;
	long duration, lastDuration;

public:
	PushButton(int p) {
		pin = p;
		status = HIGH;
	}

	PushButton(int p, boolean defaultState) {
		pin = p;
		status = defaultState;
	}

	void init() {
		pinMode(pin, INPUT);
		digitalWrite(pin, status);
		lastReading = status;
		duration = 0;
		lastDuration = 0;
	}

	void check() {
		boolean reading = digitalRead(pin);
		if (lastReading != reading) {
			if ( millis() - bounced > defaultThreshold ) {
				duration = millis() - bounced;
			}
			bounced = millis();
		}
		lastReading = reading;
	}

	long continued() {
		return lastDuration;
	}

	long continuing() {
		return millis() - bounced;
	}

	boolean changed(long thre = defaultThreshold) {
		if (millis() > bounced + thre && lastReading != status) {
			lastDuration = duration;
			duration = millis() - bounced;
			status = lastReading;
			return true;
		}
		return false;
	}

	boolean isLow() {
		return !status;
	}

	boolean isHigh() {
		return status;
	}

	boolean state() {
		return status;
	}

};

#endif /* PUSHBUTTON_H_ */
