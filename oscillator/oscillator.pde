
int osc0, osc1;
int per0, per1;
int freq0 = 125, freq1 = 126;
void setup() {
  pinMode(12, OUTPUT);
  
  per0 = freq0;
  per1 = freq1;
}

void loop() {
  per0--;
  per1--;
  if (per0 == 0) {
    per0 = freq0;
    osc0 = !osc0;
  }
  if ( per1 == 0) {
    per1 = freq1;
    osc1 = !osc1;
  }
  digitalWrite(12, (osc0 && osc1));
}

