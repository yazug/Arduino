
const int pin2A = 3;
const int pin1A = 5;

long swatch;
void setup() {
  swatch = millis();
}

int phase = 1;
int period = 1000/60;
int power = 127;

void loop() {
  if ( abs(millis() - swatch) > 1000)
  while (true);
  if (phase > 0) {
    analogWrite(pin1A, power);
    analogWrite(pin2A, 0);
    phase = -phase;
  } 
  else if (phase < 0) {
    analogWrite(pin1A, 0);
    analogWrite(pin2A, power);
    phase = -phase;
  }    
  delay(period);
}








