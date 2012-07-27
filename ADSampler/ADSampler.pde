
const int BUFFERSIZE = 320;
int buffer[BUFFERSIZE];
int sIndex;
const long sDelay = 2000;

void setup() {
  Serial.begin(9600);
  sIndex = 0;
}

void loop() {
  int i;
  
  while (analogRead(0) < 16)
    delayMicroseconds(sDelay);
    
  for (sIndex = 0; sIndex < BUFFERSIZE; sIndex++) {
    buffer[sIndex] = analogRead(0);
    delayMicroseconds(sDelay);
  }

  for (sIndex = 0; sIndex < BUFFERSIZE; sIndex++) {
    Serial.println(buffer[sIndex]);
//    delay(5);
  }

}




