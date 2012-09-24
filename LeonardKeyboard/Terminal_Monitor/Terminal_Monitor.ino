
int col;
const int col_max = 20;

void setup() {
  col = 0;
  while (!Serial);
  Serial.begin(19200);
  Serial.println("Hi.");
  Serial1.begin(19200);
}

void loop() {
  unsigned char c;
  while ( Serial1.available() ) {
    c = Serial1.read();
    Serial.print(c>>4, HEX);
    Serial.print(c&0x0f, HEX);
    Serial.print(' ');
    col++;
    if ( col >= col_max ) {
      col = 0;
      Serial.println();
    }
  }
}
