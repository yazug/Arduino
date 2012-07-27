#include <SerialMonitor.h>

SerialMonitor mon(Serial);

void setup() {
  Serial.begin(38400);
  mon << mon.endl;
}

void loop() {
  int anaval[3], anamax[3], anamin[3];
  int count, index, aval;
  const int sampleMax = 108;
  long sum[3];

  anamax[0] = analogRead(0);
  anamin[0] = anamax[0];
  sum[0] = anamax[0];
  for (count = 1; count < sampleMax; count++) {
    aval = analogRead(0);
    anamax[0] = max(aval, anamax[0]);
    anamin[0] = min(aval, anamin[0]);
    sum[0] += aval;
    delayMicroseconds(753);
  }
  anaval[0] = sum[0] * 5000 / count / 1024;
  anaval[1] = (long)analogRead(1) * 5000 / 1024;
  //  a2val = sum2 / count;

  mon << "A0: " << float(sum[0])/count 
    << " +" << (anamax[0]-float(sum[0])/count)
    << "/-" << (float(sum[0])/count - anamin[0]) << ", "
    << anaval[0] << " mV" << mon.endl;
  mon << "A1: " 
//    << " +" << (anamax[1]-sum[1])
//    << "/-" << (sum[0] - anamin[1])  << ", "
    << anaval[1] << " mV" << mon.endl << mon.endl;
  delay(2000);
}


