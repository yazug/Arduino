#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "WProgram.h"
#include "util.h"

void ROM_putstring(const char *str, uint8_t nl) {
    uint8_t i;
    
    for (i=0; pgm_read_byte(&str[i]); i++) {
      Serial.print(pgm_read_byte(&str[i]));
  }
  if (nl) {
	  Serial.println();
//    Serial.print('\n'); Serial.print('\r');
  }
}

