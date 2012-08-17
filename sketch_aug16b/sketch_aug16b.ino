
void setup() {
  pinMode(13, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, INPUT);
  digitalWrite(3, HIGH);
}

const byte PD2 = 1<<2;

void loop() {
long cnt = 0;
for(;;) {
    asm("nop");
    asm("nop");
    PORTD |= PD2;
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    PORTD &= ~(PD2);
    asm("nop");
    asm("nop");
  }
}

