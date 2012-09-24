/*

*/

void setup() {
  while (! Serial1);
  //
  Serial1.begin(9600);
  Keyboard.begin();
}

void loop() {
  while (Serial1.available() > 0) {
    int c = (char) Serial1.read();
    Keyboard.write(c);
    // Echo back
    Serial1.write(c);
  }
}

