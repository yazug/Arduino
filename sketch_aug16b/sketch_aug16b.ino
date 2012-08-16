
void setup() {
  pinMode(13, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, INPUT);
  digitalWrite(3, HIGH);
}

void loop() {
long cnt = 0;
for(;;) {
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    digitalWrite(2, HIGH);
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    digitalWrite(2, LOW);
    cnt++;
    if ( cnt & 0x10000 ) {
      digitalWrite(13, LOW);
    } else {
      digitalWrite(13, HIGH);
    }
  }
}

