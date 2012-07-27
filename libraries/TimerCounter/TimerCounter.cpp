/*
 * TimerCounter.cpp
 *
 *  Created on: 2012/02/18
 *      Author: sin
 */

#include <TimerCounter.h>

TimerCounter0 Timer0;
TimerCounter2 Timer2;

ISR(TIMER0_COMPA_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
	  Timer0.isr_OCF0A();
}

ISR(TIMER2_COMPA_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
	  Timer2.isr_OCF2A();
}

