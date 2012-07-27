
const unsigned char PortDMask = 0b11111100;

void setup() {
  int i;
  
  DDRD |= (PortDMask & 0xff); // set all the pins output except 0 & 1
  
//  Serial.begin(19200);
  
}

int cycle = 0;
unsigned char wtable[] = {
  0,
  1,
  2,3,4,5,6,7,8,9,10,11,12,13,14,15
};

void loop() {
  int value;
  
  value = wtable[cycle];
//  Serial.print(value);
//  Serial.print(" \t");
  PORTD = 0xff & (value<<2);
//  Serial.print(value, BIN);
  cycle++;
  cycle &= 0x0f;
  delayMicroseconds(80);
//  Serial.println();
}

