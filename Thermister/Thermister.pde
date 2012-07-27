
inline int thermister(byte n) {
  switch (n) {
    case 0:
    default:
    return analogRead(2);
  }
}

long interval = 0;
void setup() {
  Serial.begin(9600);
}

void loop() {
  if ( millis() - interval > 1000 ) {
    Serial.println(thermister(0));
    interval = millis();
  }
}

