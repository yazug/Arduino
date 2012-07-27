/*
 * Timer.h
 *
 *  Created on: 2012/02/18
 *      Author: sin
 *
 *      Provides Access structure for Timer/Counter 0,1,2
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "Arduino.h"

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class TimerCounter {
public:
	byte id;
	volatile byte & tccra;
	volatile byte & tccrb;
	volatile byte & clksel;
	volatile byte & tintmask;
	volatile byte & tcnt;
	volatile byte & outcomp;
	byte ocf;
	byte toie;
	byte clksel_value;

public:
	TimerCounter(byte id,
			volatile unsigned char & tccrxa,
			volatile unsigned char & tccrxb,
			volatile unsigned char & clockselect,
			volatile unsigned char & timask,
			volatile unsigned char & ocrxa,
			volatile unsigned char & tcntx,
			unsigned char toiex,
			unsigned char ocfxa) :
	 id(id), tccra(tccrxa), tccrb(tccrxb), clksel(clockselect), tintmask(timask),
	 outcomp(ocrxa), tcnt(tcntx), toie(toiex), ocf(ocfxa) {
	 clksel_value = 0;
	 }
	~TimerCounter() {}

public:
	static const byte TC0_WGM_CTC = 2;
	static const byte TC2_WGM_CTC = 0b010;

	void WaveGenerationMode(byte wgmx) {
		bitWrite(tccra, 0, wgmx & 1);
		bitWrite(tccra, 1, wgmx>>1 & 1);
		bitWrite(tccrb, 3, wgmx>>2 & 1);
	}

	inline void CTCmode() {
		WaveGenerationMode(TC0_WGM_CTC);
	}

	virtual void stop();

	inline void start(const byte cs) {
		clksel_value = cs;
		ClockSelect(cs);
	}

	inline void restart() {
		ClockSelect(cs_value);
	}

	void ClockSelect(byte csb) {
		bitWrite(clksel, 0, csb & 1);
		bitWrite(clksel, 1, csb>>1 & 1);
		bitWrite(clksel, 2, csb>>2 & 1);
	}

	void setCompareA(byte val) {
		outcomp = val;
	}

	void OverflowInterrupt(boolean enable) {
		bitWrite(tintmask, toie, enable);
	}

	void OutputCompareMatchAInterrupt(boolean enable) {
		bitWrite(tintmask, ocf, enable);
	}

	byte counter() {
		return tcnt;
	}

	byte counter(byte val) {
		stop();
		tcnt = val;
		ClockSelect(cs_value);
		return tcnt;
	}

	void (*isr_OCF0A)();
	void (*isr_OCF2A)();

	void attachOCMAInterrupt(void (*isr)()) {
		switch (id) {
		case 0:
			isr_OCF0A = isr; // register the user's callback with the real ISR
			break;
		case 2:
			isr_OCF2A = isr; // register the user's callback with the real ISR
			break;
		}
		OutputCompareMatchAInterrupt(true); // sets the timer overflow interrupt enable bit
		sei();
		// ensures that interrupts are globally enabled
//    start();

	}

	void detachOutputCompareMatchAInterrupt() {
		OutputCompareMatchAInterrupt(false);
	}

};

class TimerCounter0: public TimerCounter {
public:
	enum {
		CS0x_STOP = 0,
		CS0x_1 = 1,
		CS0x_DIV8 = 2,
		CS0x_DIV64 = 3,
		CS0x_DIV256 = 4,
		CS0x_DIV1024 = 5,
		CS0x_EXTFALLING = 6,
		CS0x_EXTRISING = 7
	};

	TimerCounter0() :
		TimerCounter( byte(0), TCCR0A, TCCR0B, TCCR0A, TIMSK0, OCR0A, TCNT0,
				byte(TOIE0), byte(OCF0A) ) {
		clksel_value = CS0x_STOP;
	}

	inline void stop() {
		ClockSelect(CS0x_STOP);
	}
};

class TimerCounter2: public TimerCounter {
public:
	enum {
		CS2x_STOP = 0,
		CS2x_1 = 1,
		CS2x_DIV8 = 2,
		CS2x_DIV32 = 3,
		CS2x_DIV64 = 4,
		CS2x_DIV128 = 5,
		CS2x_DIV256 = 6,
		CS2x_DIV1024 = 7,
	};

	TimerCounter2() :
		 TimerCounter( byte(2), TCCR2A, TCCR2B, TCCR2A, TIMSK2, OCR2A, TCNT2,
				 byte(TOIE2), byte(OCF2A) ) {
		clksel_value = CS2x_STOP;
	}

	inline void stop() {
		ClockSelect(CS2x_STOP);
	}

};

extern TimerCounter0 Timer0;
extern TimerCounter2 Timer2;

#endif /* TIMER_H_ */
