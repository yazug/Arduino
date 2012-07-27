const int IR_GND = 14;
const int IR_OUT = 15;
const int IR_VCC = 16;
inline byte dread() { 
  return (PORTC & PORTC2) == 0; 
}

enum STATE{
  Lo = LOW, 
  Hi = HIGH,
  SartOver
} 
IRstate;
int duration_low, duration_high;
int last_falling, last_rising;

void setup() {
  pinMode(IR_GND, OUTPUT);
  digitalWrite(IR_GND, LOW);
  pinMode(IR_OUT, INPUT);
  digitalWrite(IR_OUT, HIGH);
  pinMode(IR_VCC, OUTPUT);
  digitalWrite(IR_VCC, HIGH);

  Serial.begin(19200);
  Serial.println();
  Serial.println("Start.");

  // init
  duration_low = 0;
  duration_high = 0;
  last_falling = 0;
  last_rising = 0;
  IRstate = (STATE) digitalRead(IR_OUT);
}

void loop() {
  byte b = 0;
  if ( digitalRead(IR_OUT) == LOW ) {
    while ( digitalRead(IR_OUT) == LOW );
    long eighth = micros()>>7; // the last rising 
    long duration;
    while ( digitalRead(IR_OUT) == HIGH ) {
      duration = (micros()>>7) - eighth;
      if ( duration > 45 ) {
        break;
      }
    }
    if ( digitalRead(IR_OUT) == HIGH ) {
      // if still high
      byte b = read_IR();
      if ( b ) Serial.println(b, DEC);
    }
  }
}

byte read_IR() {
  byte value = 0;
  int bits = 0;
  long lastRising = micros()>>7;
  long lastFalling;
  int dhigh, dlow;
  if ( digitalRead(IR_OUT) != HIGH ) 
    return 0;
  while ( digitalRead(IR_OUT) == HIGH ) { 
    if ( micros()>>7 > lastRising + 480 ) return 0; 
  }
  lastFalling = micros()>>7;
  dhigh = lastFalling - lastRising;
  if ( dhigh < 48 )
    return 0;
  dhigh = 48;
  do {
    while ( digitalRead(IR_OUT) == LOW )  { 
      if ( micros()>>7 > lastFalling + 160 ) return 0; 
    }
    lastRising = micros()>>7;
    dlow = lastRising - lastFalling;
    while ( digitalRead(IR_OUT) == HIGH )  { 
      if ( micros()>>7 > lastRising + 160 ) {
        if ( bits == 7 ) break;
        return 0; 
      }
    }
    lastFalling = micros()>>7;
    dhigh = lastFalling - lastRising;
    value <<= 1;
    if ( (dlow + 3)>>3 == 3 ) {
      //      Serial.print('1');
      value |= 1;
    }
    bits++;
  } 
  while (bits < 8);

  //  Serial.println();
  return value;
}







