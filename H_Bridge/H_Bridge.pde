
const int pinAC0 = 3;
const int pinAC1 = 9;
const int pinSW1 = 10;
const int pinSW2 = 8;

const int pinPOT1 = 2;
const int pinPOT2 = 3;

const int minSecs = 6;
const int overlap = 6;

boolean acOn;

long emillis;
long emicros;
int eseconds;

void setup() {
  int a1, a2;

  pinMode(pinSW1, OUTPUT);
  pinMode(pinSW2, OUTPUT);
  pinMode(13, OUTPUT);

  emicros = micros();
  acOn = false;

  a1 = max(minSecs, analogRead(pinPOT1)>>3);
  a2 = max(minSecs, analogRead(pinPOT2)>>3);

  Serial.begin(9600);
  Serial.print("ADC1:");
  Serial.println(a1);
  Serial.print("ADC2:");
  Serial.println(a2);
}

int ACphase = 0;
const int microstep = 1900;
#define SLOWERING (600)
int ACpower[] = { 
  157, 255, 255, 157, 0, -157, -255, -255, -157, 0};

int slowering = SLOWERING;
int adc1 = minSecs, adc2 = minSecs;

void loop() {
  int a1, a2;

  if ( acOn ) {
    if ( micros() - emicros > microstep) {
      if (slowering > 0) slowering--;
      emicros = micros() + slowering;
      ACphase++;
      ACphase %= 10;
    }
    analogWrite(pinAC0, min(255, 255 + ACpower[ACphase]));
    analogWrite(pinAC1, min(255, 255 - ACpower[ACphase]));
  } 
  else {
    analogWrite(pinAC0, 0);
    analogWrite(pinAC1, 0);
    slowering = SLOWERING;
  }

  a1 = max(minSecs, analogRead(pinPOT1)>>3);
  a2 = max(minSecs, analogRead(pinPOT2)>>3);

  if ( millis() - emillis > 1000 ) {
    emillis = millis();
    eseconds++;
  } 
  else {
    return;
  }

  if (eseconds < a1 || eseconds > a1+a2 - overlap) {
    digitalWrite(13, HIGH);
    digitalWrite(pinSW1, HIGH);
  }
  else {
    digitalWrite(13, LOW);
    digitalWrite(pinSW1, LOW);
  } 
  if ( eseconds < a1 - overlap ) {
    acOn = false;
    digitalWrite(pinSW2, LOW);
  } 
  else {
    acOn = true;
    digitalWrite(pinSW2, HIGH);
  }
  if (eseconds > a1+a2 ) {
    eseconds = 0;
  }
}



















