
const int ledPins[] = { 
  8, 9, 10, 11 };
const int NofLEDs = sizeof(ledPins)/sizeof(int);
const int swPin = 12;

int ledStates[NofLEDs];
enum {
  LED_OFF = 0,
  LED_BLINK,
  LED_BLINK_FAST,
  LED_ON
};

long milliCounter, secCounter;
int timerMinutes;
boolean timerStop;

void setup() {
  int i;
  for (i = 0; i < NofLEDs; i++) {
    pinMode(ledPins[i], OUTPUT);
    ledStates[i] = LED_OFF;
  }
  pinMode(swPin, INPUT);
  digitalWrite(swPin, HIGH); // pull-pu swPin
  
  setLed();
  timerMinutes = 3;
  secCounter = 0;
  timerStop = false;
}

void setLed() {
  int i;
  for (i = 0; i < NofLEDs; i++) {
    switch(ledStates[i]) {
    case LED_OFF:
      digitalWrite(ledPins[i], LOW);
      break;
    case LED_ON:
      digitalWrite(ledPins[i], HIGH);
      break;
    }
  }
}

void loop() {
  int i;
  long buttonMillis;
  
  if ( digitalRead(swPin) == LOW ) {
    buttonMillis = millis();
    while ( digitalRead(swPin) == LOW );
    // momentary pushed
    if ( millis() - buttonMillis < 500 ) {
      timerStop = !timerStop;
      if ( !timerStop ) {
        milliCounter = millis();
      }
    }
  }
  
  if ( !timerStop ) {
    if ( millis() - milliCounter >= 1000 ) {
      milliCounter = millis() - (milliCounter - 1000);
      secCounter++;
    }
  }
  
  for (i = 0; i < NofLEDs; i++) {
    if ( secCounter > i*60 ) 
      ledStates[i] = LED_ON;
  }
  setLed();
}


