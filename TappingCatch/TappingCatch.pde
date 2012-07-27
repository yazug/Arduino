#include <avr/delay.h>
int a0val;
long lastmillis;
boolean listening = false;
byte seq[16];
int intens[17];
int q, maxdur;

const int waitQuiet = 48; // 48 for vibration sensor
const int minLEDupdate = 50;
const int silenceThreshold = 1600;
const int activeLevel = 10;

void setup() {
  pinMode(3, INPUT);
  pinMode(4, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(38400);

  lastmillis = millis();
}

void loop() {
  long diff = millis() - lastmillis;
  
  if ( diff == 0 ) 
    return;

  // update the state of led
  if ( diff > minLEDupdate ) {
    digitalWrite(4, LOW);
  }

  if ( listening && (diff > silenceThreshold || ((maxdur != 0) && diff > maxdur*3 )) ) {
    listening = false;
    digitalWrite(13, LOW);
    printSequence(q);
  }

  a0val = analogRead(0);

  if ( a0val > activeLevel ) {
    if ( !listening ) {
      listening = true;
      lastmillis = millis();
      q = 0;
      intens[q] = a0val;
      maxdur = 0;
      digitalWrite(4,HIGH);
      digitalWrite(13, HIGH);
    } 
    else {
      if ( diff > waitQuiet ) {
        seq[q++] = diff>>4;
        intens[q] = a0val;
        maxdur = max(maxdur, diff);
        digitalWrite(4,HIGH);
        digitalWrite(13, HIGH);
      }
      lastmillis = millis();

    }
  }

}


void printSequence(int q) {
  int i;
  if ( !( q > 0 ) )
    return;
  int dmax = seq[0], dmin = dmax;
  int base;
  for (i = 0; i < q; i++) {
    dmax = max(seq[i],dmax);
    dmin = min(seq[i], dmin);
  }
  Serial.print("dmin: ");
  Serial.print(dmin);
  Serial.print(", dmax: ");
  Serial.print(dmax);
  base = dmax/((int)(0.5 + dmax/(float)dmin));
  Serial.print(", base: ");
  Serial.println(base);
  for (i = 0; i < q; i++) {
    seq[i] = (int)(seq[i]/(float)base + 0.5);
    Serial.print(seq[i]);
    Serial.print(" ");
  }
  Serial.println();
  seq[i] = 0;

  for (i = 0; i < q; i++) {
    Serial.print("[");
    Serial.print(intens[i], DEC);
    Serial.print("] -");
    Serial.print(seq[i], DEC);
    Serial.print("- ");
  }
  Serial.print("[");
  Serial.print(intens[i], DEC);
  Serial.print("] ");
  Serial.println();
  
  byte txt[] = {1, 1, 2, 0 };
  for (i = 0; seq[i] != 0 && txt[i] != 0; i++) {
    if (txt[i] != seq[i])
      break;
  }
  if (seq[i] == 0 && txt[i] == 0) {
    digitalWrite(3, HIGH);
    delay(8000);
    digitalWrite(3, LOW);
  }
  
}





