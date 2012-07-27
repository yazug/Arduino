const int samples = 32;
struct {
  int val, avr, maxv, minv;
} hist[samples];
int q;

void setup() {
  // we use a0 for reading the sensor output
  // but for it no set-up is required.
  Serial.begin(19200);
}

void loop() {
  int i, sum;
  int val = analogRead(0);
  q = (q+1) % samples;
  hist[q].val = val;
  hist[q].maxv = val;
  hist[q].minv = val;
  sum = 0;
  for(i = 0; i < samples; i++) {
    sum += hist[i].val;
    hist[q].maxv = max(hist[q].maxv, hist[i].val);
    hist[q].minv = min(hist[q].minv, hist[i].val);    
  }
  hist[q].avr = sum/samples;
  if (q > 0) {
    delay(13);
    return;
  }
  
  Serial.print("avr: ");
  Serial.print(hist[q].avr );
  Serial.print(", min: ");
  Serial.print(hist[q].minv);
  Serial.print(", max: ");
  Serial.print(hist[q].maxv);
  Serial.print(", +diff: ");
  Serial.print((hist[q].maxv-hist[q].minv)*5.0/1024, 3);
  Serial.println(" A.");
  
  /*
  for (i = 0; i < (hist[q].maxv-hist[q].avr)*5.0/1.024; i++) {
    Serial.print("+");
  }
  Serial.println();
  */
  delay(1000);
}





