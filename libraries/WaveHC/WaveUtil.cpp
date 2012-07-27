#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "WProgram.h"
#include "WaveUtil.h"

#ifdef OLD
void ROM_putstring(const char *str, uint8_t nl) {
    uint8_t i;
    
    for (i=0; pgm_read_byte(&str[i]); i++) {
      Serial.print(pgm_read_byte(&str[i]));
  }
  if (nl) {
    Serial.print('\n'); Serial.print('\r');
  }
}
#else

void ROM_putstring(const char *str) {
  for (uint8_t c, i=0; (c = pgm_read_byte(&str[i])); i++) Serial.print(c);
}

void ROM_putstringnl(const char *str) {
  ROM_putstring(str);
  Serial.println();
}
#endif
