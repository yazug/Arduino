/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/

#include "ch.h"
#include "hal.h"
/**
 * @brief   PAL setup.
 * @details Digital I/O ports static configuration as defined in @p board.h.
 *          This variable is used by the HAL when initializing the PAL driver.
 */
#if HAL_USE_PAL || defined(__DOXYGEN__)
const PALConfig pal_default_config =
{
#if defined(PORTA)
  {VAL_PORTA, VAL_DDRA},
#endif
#if defined(PORTB)
  {VAL_PORTB, VAL_DDRB},
#endif
#if defined(PORTC)
  {VAL_PORTC, VAL_DDRC},
#endif
#if defined(PORTD)
  {VAL_PORTD, VAL_DDRD},
#endif
#if defined(PORTE)
  {VAL_PORTE, VAL_DDRE},
#endif
#if defined(PORTF)
  {VAL_PORTF, VAL_DDRF},
#endif
#if defined(PORTG)
  {VAL_PORTG, VAL_DDRG},
#endif
};
#endif /* HAL_USE_PAL */
/**
 * @brief   Timer0 interrupt handler.
 */
CH_IRQ_HANDLER(TIMER0_COMPA_vect) {
  CH_IRQ_PROLOGUE();

  chSysLockFromIsr();
  chSysTimerHandlerI();
  chSysUnlockFromIsr();
  CH_IRQ_EPILOGUE();
}
/*
 * Board-specific initialization code.
 */
void boardInit(void) {
  /*
   * Timer 0 setup.
   */
  OCR0A = 128;
  TIMSK0  |= (1 << OCIE0A);                               /* IRQ on compare.  */
}
