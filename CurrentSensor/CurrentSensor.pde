
const int SAMPLESIZE = 48;
int index;
int samples[SAMPLESIZE];
void setup() {
  Serial.begin(9600);
}

void loop() {
  int i;
  int maximum, minimum;
 long avr;
  for (index = 0; index < SAMPLESIZE; index++) {
    samples[index] = analogRead(0);
    delayMicroseconds(1023);
  }

  maximum = minimum = samples[0];
  avr = 0;
  for (i = 0; i < SAMPLESIZE; i++) {
    maximum = max(samples[i], maximum);
    minimum = min(samples[i], minimum);
    avr += samples[i];
  }
//  Serial.print(minimum);
//  Serial.print(" -- ");
//  Serial.print(maximum);
  Serial.print("AVR: \t");
  Serial.print((float)avr/SAMPLESIZE, 1);
  Serial.print("\t ABS DIFF: \t");
  Serial.print(maximum - minimum);
  Serial.println();

  delay(1000);
}


