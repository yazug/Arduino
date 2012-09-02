/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.h 249 2007-02-03 16:52:51Z mellis $
*/

// Quaranta

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>

#define digitalPinHasPWM(p)         ((p) == 4 || (p) == 5 || (p) == 6 || (p) == 7 || (p) == 9 || (p) == 10)

#define NUM_DIGITAL_PINS            32
#define NUM_ANALOG_INPUTS           8
#define analogInputToDigitalPin(p)  ((p < 8) ? (p) + 24 : -1)

static const uint8_t SDA = 16; // PC1
static const uint8_t SCL = 17; // PC0

// Map SPI port
static const uint8_t SS   = 10; // PB4
static const uint8_t MOSI = 11;
static const uint8_t MISO = 12;
static const uint8_t SCK  = 13; // PB7
static const uint8_t LED_BUILTIN = 14;


// Mapping of analog pins as digital I/O
static const uint8_t A0 = 24; //PA0
static const uint8_t A1 = 25;
static const uint8_t A2 = 26;
static const uint8_t A3 = 27;
static const uint8_t A4 = 28;
static const uint8_t A5 = 29;
static const uint8_t A6 = 30;
static const uint8_t A7 = 31;

#define digitalPinToPCICR(p)    (((p) >= 0 && (p) <= 32) ? (&PCICR) : ((uint8_t *)0))
#define digitalPinToPCICRbit(p) (((p) <= 7) ? 3 : (((p) <= 15) ? 2 : (((p) <= 23) ? 1 : 0 ) ))
#define digitalPinToPCMSK(p)    (((p) <= 7) ? (&PCMSK3) : (((p) <= 15) ? (&PCMSK2) : (((p) <= 23) ? (&PCMSK1) : (&PCMSK0) )))
#define digitalPinToPCMSKbit(p) (((p) <= 7) ? (p) : (((p) <= 15) ? ((p) - 8) : (((p) <= 23) ? ((p) - 16) : ((p) - 24) )  ))

//	__AVR_ATmega32U4__ has an unusual mapping of pins to channels
extern const uint8_t PROGMEM analog_pin_to_channel_PGM[];
#define analogPinToChannel(P)  ( pgm_read_byte( analog_pin_to_channel_PGM + (P) ) )

#ifdef ARDUINO_MAIN

// ATMEL ATMEGA644P / Quaranta
//
//                   +---\/---+
//       (D14) PB0  1|        |40  PA0 (AI 0 / D24)
//       (D15) PB1  2|        |39  PA1 (AI 1 / D25)
//       (D 8) PB2  3|        |38  PA2 (AI 2 / D26)
//   PWM (D 9) PB3  4|        |37  PA3 (AI 3 / D27)
//   PWM (D10) PB4  5|        |36  PA4 (AI 4 / D28)
//  MOSI (D11) PB5  6|        |35  PA5 (AI 5 / D29)
//  MISO (D12) PB6  7|        |34  PA6 (AI 6 / D30)
//   SCK (D13) PB7  8|        |33  PA7 (AI 7 / D31)
//             RST  9|        |32  AREF
//             VCC 10|        |31  GND
//             GND 11|        |30  AVCC
//           XTAL2 12|        |29  PC7 (D 23)
//           XTAL1 13|        |28  PC6 (D 22)
//  RX0 (D 0)  PD0 14|        |27  PC5 (D 21) TDI
//  TX0 (D 1)  PD1 15|        |26  PC4 (D 20) TDO
//  RX1/INT0 (D 2)  PD2 16|        |25  PC3 (D 19) TMS
//  TX1/INT1 (D 3)  PD3 17|        |24  PC2 (D 18) TCK
//  PWM (D 4)  PD4 18|        |23  PC1 (D 17) SDA
//  PWM (D 5)  PD5 19|        |22  PC0 (D 16) SCL
//  PWM (D 6)  PD6 20|        |21  PD7 (D  7) PWM
//                   +--------+
//

// these arrays map port names (e.g. port B) to the
// appropriate addresses for various functions (e.g. reading
// and writing)
const uint16_t PROGMEM port_to_mode_PGM[] = {
	NOT_A_PORT,
	(uint16_t) &DDRA,
	(uint16_t) &DDRB,
	(uint16_t) &DDRC,
	(uint16_t) &DDRD,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
	NOT_A_PORT,
	(uint16_t) &PORTA,
	(uint16_t) &PORTB,
	(uint16_t) &PORTC,
	(uint16_t) &PORTD,
};

const uint16_t PROGMEM port_to_input_PGM[] = {
	NOT_A_PORT,
	(uint16_t) &PINA,
	(uint16_t) &PINB,
	(uint16_t) &PINC,
	(uint16_t) &PIND,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
	PD, /* 0 */
	PD,
	PD,
	PD,
	PD,
	PD,
	PD,
	PD,
	PB, /* 8 */
	PB,
	PB,
	PB,
	PB,
	PB,
	PB,
	PB,
	PC, /* 16 */
	PC,
	PC,
	PC,
	PC,
	PC,
	PC,
	PC,
	PA,
	PA,
	PA,
	PA,
	PA,
	PA,
	PA,
	PA,
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
	_BV(0), /* 0, port D */
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5),
	_BV(6),
	_BV(7),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5),
	_BV(6),
	_BV(7),
	_BV(0), /* 8, port B */
	_BV(1),
	_BV(0), /* 16, port C */
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5),
	_BV(6),
	_BV(7),
	_BV(0), /* 24, port A */
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5),
	_BV(6),
	_BV(7),
};

const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
	NOT_ON_TIMER, /* 0 - port D */
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	TIMER1B,
	TIMER1A,
	TIMER2B,
	TIMER2A,
	NOT_ON_TIMER, /* 8 - port B */
	TIMER0A,
	TIMER0B,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER, /* 8 - port B */
	NOT_ON_TIMER,
	NOT_ON_TIMER, /* 16 - port C */
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER, /* 24 - port A */
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
};

#endif /* ARDUINO_MAIN */
#endif /* Pins_Arduino_h */
