
const int pin_IR_led = 3;
const int pin_monitor_led = 13;

void IRout(boolean hilo) {
  digitalWrite(3, hilo);
  digitalWrite(13, hilo);
}

void setup() {
  pinMode(3, OUTPUT);
  pinMode(13, OUTPUT);
  
}

void loop() {
  IRout(HIGH);
  delay(200);
  IRout(LOW);
  delay(200);
}

