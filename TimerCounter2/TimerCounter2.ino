#define F_CPU 8000000UL
#include <TimerCounter.h>
#include <util/delay.h>

volatile byte comp2aCount;
volatile boolean IRstate;

void comp2aInterruptService() {
  if ( IRstate ) 
    PORTD ^= (1<<3);
  else
    PORTD &= ~(1<<3);
  comp2aCount++;
  //  bitWrite(PORTD, 3, comp2aCount & IRstate);
  //digitalWrite(3, pulseToggle && IRstate);
}

void IR_high() {
  IRstate = true;
  digitalWrite(13, HIGH);
}

void IR_low() {
  IRstate = false;
  digitalWrite(13, LOW);
}

void setup() {
  Timer2.stop();
  Timer2.mode_CTC();
  Timer2.setCompareA(13);
  Timer2.start(Timer2.CS2x_DIV8);

  pinMode(3, OUTPUT);
  pinMode(13, OUTPUT);
  Timer2.attachOutputCompareMatchAInterrupt(comp2aInterruptService);
  //  sei();
}

void loop() {
  byte countCopy;
  const char mess[] = { 
    133, 'A', 211, 220, 0        };
  for (int i = 0; mess[i] != 0; i++) {
    byte d = mess[i] | 0x80;
    for (int b = 0; b < 8; b++) {
      byte mask = (0x80 >> b);
      if ( (d & mask) == 0 ) {
        IR_high();
        comp2aCount = 0;
        while ( comp2aCount < 71);
        IR_low();
        comp2aCount = 0;
        while ( comp2aCount < 210);
      } 
      else {
        IR_high();
        comp2aCount = 0;
        while ( comp2aCount < 211);
        IR_low();
        comp2aCount = 0;
        while ( comp2aCount < 70);
      }
    }
    delay(10);
  }
  delay(250);
}






