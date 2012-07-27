
#define AREF_V 1096   // the value for INTERNAL AREF Voltage
//#define AREF_V 5125

long a0avr;

void setup() {
  Serial.begin(9600);
}

void loop() {
  analogReference(INTERNAL);
  a0avr = (analogRead(0) + a0avr)/2;
  Serial.println( a0avr * AREF_V /1024 / 6.25 - 67.84 );
  delay(750);
}

