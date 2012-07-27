
unsigned long rtcounter;
void setup() {
  Serial.begin(38400);
  rtcounter = millis();
}

void loop() {
  if ( millis() - rtcounter > 999 ) {
    rtcounter = millis();
    Serial.println(rtcounter/(float)1000, 3);
  }
}

