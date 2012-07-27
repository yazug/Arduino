long watch = millis();

void setup() {
  Serial.begin(9600);
}

void loop() {
  if ( millis() - watch > 1000 ) {
    Serial.println((analogRead(1)*3.223 - 512)/10 + 0.05, 1);
    watch = millis();
  }
}


