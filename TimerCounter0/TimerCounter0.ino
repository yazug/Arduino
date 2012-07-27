#include <TimerCounter.h>
#include <util/delay.h>

volatile boolean pulseToggle;
volatile boolean IRstate;

void comp0aInterruptService() {  
  pulseToggle = !pulseToggle;
  if ( IRstate && pulseToggle ) {
    PORTD |= 1<<3;
  } else {
    PORTD &= ~(1<<3);
  }
  //bitWrite(PORTD, 3, pulseToggle && IRstate);
  //digitalWrite(3, pulseToggle && IRstate);
}

void IRout(boolean hilo) {
  IRstate = hilo;
  digitalWrite(13, hilo);
}

void setup() {
  Timer0.stop();
  Timer0.CTCmode();
  Timer0.setCompareA(13);
  Timer0.start(Timer0.CS0x_DIV8);

  pinMode(3, OUTPUT);
  pinMode(13, OUTPUT);
  Timer0.attachOCMAInterrupt(comp0aInterruptService);
//  sei();
}

void loop() {
  const char mess[] = { 
    133, 'A', 211, 220, 0    };
  for (int i = 0; mess[i] != 0; i++) {
    byte d = mess[i] | 0x80;
    for (int b = 0; b < 8; b++) {
      byte mask = (0x80 >> b);
      if ( (d & mask) == 0 ) {
        IRout(HIGH);
        _delay_ms(1);//ocf2aCounter = 0; while ( ocf2aCounter < 65 );
        IRout(LOW);
        _delay_ms(3);//ocf2aCounter = 0; while ( ocf2aCounter < 197 );
      } 
      else {
        IRout(HIGH);
        _delay_ms(3);//ocf2aCounter = 0; while ( ocf2aCounter < 197 );
        IRout(LOW);
        _delay_ms(1);//ocf2aCounter = 0; while ( ocf2aCounter < 65 );
      }
    }
    _delay_ms(10);
  }
  _delay_ms(250);
}




