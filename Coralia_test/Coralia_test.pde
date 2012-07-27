const int pinIN1 = 3;
const int pinIN2 = 9;

int acphase;
long timer;
long emicros;
long esecs;
int power = 255;
const int microstep = 2800;
int slowering = 1000;

void setup() {
  acphase = 0;
  timer = millis();
}

void loop() {
  if (millis() - timer > 10000) {
    analogWrite(pinIN1, 0);
    analogWrite(pinIN2, 0);
    while (true);
    return;
  }
  if ( micros() - emicros > microstep + slowering) {
    emicros = micros();
    if ( slowering > 0 )
      slowering--;
    acphase++;
  }
  switch(acphase) {
  case 1:
  case 2:
    analogWrite(pinIN1, 255);
    analogWrite(pinIN2, 0);
    break;
  case 3:
    analogWrite(pinIN1, 255);
    analogWrite(pinIN2, 255);
    break;
  case 4:
  case 5:
    analogWrite(pinIN1, 0);
    analogWrite(pinIN2, 255);
    break;
  default:
    analogWrite(pinIN1, 255);
    analogWrite(pinIN2, 255);
    acphase = 0;
    break;
  }
}







