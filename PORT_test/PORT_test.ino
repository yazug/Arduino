
struct Port {
  volatile unsigned char & outport;
  volatile unsigned char & dirport;
  volatile unsigned char & inport;
  byte bitmask;
  
  void output(byte m) {
    dirport |= m;
  }
  
  void input(byte m) {
    dirport &= ~m;
  }
  
  void write(byte d) {
    //iodir |= bitmask;
    outport &= ~bitmask;
    outport |= bitmask & d;
  }
};

void setup() {
  Port PD2 = {PORTD, DDRD, PIND, 1<<2};
  Port PD3 = {PORTD, DDRD, PIND, 1<<3};
  
  PD2.output(1<<2);
  PD3.input(1<<3);
  
  for(;;) {
    PD2.write(1<<2);
    asm("nop");
    asm("nop");
    PD2.write(0<<2);
    asm("nop");
  }
}

void loop() {
}

