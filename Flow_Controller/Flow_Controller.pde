

const int pinSW1 = 10;
const int pinSW2 = 8;
const int ainPOT1 = 2;
const int ainPOT2 = 3;

const int minInterval = 6;

int phase;
long secs, prevsecs;

void setup() {
  pinMode(pinSW1, OUTPUT);
  pinMode(pinSW2, OUTPUT);
  pinMode(13, OUTPUT);

  Serial.begin(9600);
  phase = 0;
  secs = millis()/1000;
  prevsecs = secs;
}

void loop() {
  int ain1, ain2;

  secs = millis()/1000;
  ain1 = max(minInterval, analogRead(ainPOT1)>>3);
  ain2 = max(minInterval, analogRead(ainPOT2)>>3);

  if ( secs > prevsecs + ain1) {
    prevsecs = secs;
    phase = (phase+1) % 8;
    Serial.println("ain1:");
    Serial.println(ain1);
    Serial.print("Phase ");
    Serial.println(phase);
    Serial.println();

    digitalWrite(13, LOW);
  } 
  switch (phase) {
  case 0:
    digitalWrite(pinSW1, LOW);
    digitalWrite(pinSW2, LOW);
    if ( (millis()>>10)%2 )
      digitalWrite(13, HIGH);
    else
      digitalWrite(13, LOW);
    break;
  case 1:
  case 7:
    digitalWrite(pinSW1, HIGH);
    digitalWrite(pinSW2, LOW);
    break;
  case 2:
  case 6:
    digitalWrite(pinSW1, HIGH);
    digitalWrite(pinSW2, HIGH);
    break;
  case 3:
  case 5:
    digitalWrite(pinSW1, LOW);
    digitalWrite(pinSW2, HIGH);
    break;
  case 4:
    digitalWrite(pinSW1, LOW);
    digitalWrite(pinSW2, LOW);
    break;
  }
}





