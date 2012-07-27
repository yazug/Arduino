


void setup() {
  Serial.begin(19200);
  Serial.println("Hi.");
}

void loop() {
  int i;
  const int SAMPLESIZE = 25;
  struct {
    int a0, a1, a2;
  } sample[SAMPLESIZE];
  
  for(i = 0; i < SAMPLESIZE; i++) {
    sample[i].a0 = analogRead(0);
    sample[i].a1 = analogRead(1);
    sample[i].a2 = analogRead(2);
    delayMicroseconds(1000);
  }
  long a0sum = 0, a1sum = 0, a2sum = 0;
  for (i = 0; i < SAMPLESIZE; i++) {
    a0sum += sample[i].a0;
    a1sum += sample[i].a1;
    a2sum += sample[i].a2;
  }
  float a0avr = a0sum/float(SAMPLESIZE);
  float a1avr = a1sum/float(SAMPLESIZE);
  float a2avr = a2sum/float(SAMPLESIZE);
  a0sum = 0; a1sum = 0; a2sum = 0;
  int a0dmax = 0, a1dmax = 0, a2dmax = 0;
  for (i = 0; i < SAMPLESIZE; i++) {
    a0sum += abs(sample[i].a0 - a0avr);
    a0dmax = max(a0dmax, abs(sample[i].a0 - a0avr));
    a1sum += abs(sample[i].a1 - a1avr);
    a1dmax = max(a1dmax, abs(sample[i].a1 - a1avr));
    a2sum += abs(sample[i].a2 - a2avr);
    a2dmax = max(a2dmax, abs(sample[i].a2 - a2avr));
  }
    
  Serial.print("a0 = ");
  Serial.print(a0sum*16.7/float(SAMPLESIZE), 2);
  Serial.print(" (avr) ");
  Serial.print(a0dmax);
  Serial.print(" @ ");
  Serial.print(a0avr, 1);
  Serial.print(", a1 = ");
  Serial.print(a1sum*16.7/float(SAMPLESIZE)/3.894, 2);
  Serial.print(" (avr) ");
  Serial.print(a1dmax/3.894, 1);
  Serial.print(" @ ");
  Serial.print(a1avr, 1);
  Serial.print(", a2 = ");
  Serial.print(a2sum/float(SAMPLESIZE), 2);
  Serial.print(" (avr) ");
  Serial.print(a2dmax);
  Serial.print(" @ ");
  Serial.print(a2avr, 1);
  Serial.println();
  
  delay(2000);
}

