int freq = 200;
int index = 0;
void setup() {
  pinMode(3, OUTPUT);
  Serial.begin(19200);
  Serial.print("Hi.");
}

void loop() {
  char buf[32], *ptr = buf;
  char c;
  int f;
  if (Serial.available()) {
    *ptr = 0;
    while ( Serial.available() ) {
      c = Serial.read();
      *ptr = c;
      if ( c == '\n' ) {
        *ptr = 0;
        break;
      }
      ptr++;
      while ( !Serial.available() );
    }
    f = atoi(buf);
      Serial.println(buf);
    if ( f > 0 ) {
      tone(3,f);
    }
  }
}




