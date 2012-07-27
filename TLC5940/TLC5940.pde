
const byte GSCLK = 8;
const byte BLANK = 10;
const byte SIN = 11;
const byte SCLK = 13;
const byte XLAT = 9;

void setup() {
  pinMode(BLANK,OUTPUT);
  pinMode(SIN,OUTPUT);
  pinMode(SCLK,OUTPUT);
  pinMode(XLAT,OUTPUT);
  pinMode(GSCLK,OUTPUT);
  digitalWrite(BLANK,HIGH);

  int i, b;
   word val;
  
  val = analogRead(0);
  val <<= 2;
  for (i = 0; i < 16; i++) {
    for (b = 12; b > 0; ) {
      b--;
      digitalWrite(SCLK, LOW);
      digitalWrite(SIN, (val>>b) & 1 );
      digitalWrite(SCLK, HIGH);
    }
    digitalWrite(XLAT,LOW);
    digitalWrite(XLAT,HIGH);
  }

}

int cnt = 0;
byte pwmclk = 0;
void loop() {
  word data[] = {
    0, 1, 2, 2, 3, 13, 31, 3, 3, 3, 3, 3, 3, 3, 3, 3
  };
  
  cnt++;
  if (cnt > 4095) {
    cnt = 0;
    digitalWrite(BLANK,HIGH);
    digitalWrite(BLANK,LOW);    
  }
  digitalWrite(GSCLK,pwmclk & 1);
  pwmclk = ~pwmclk;
}

