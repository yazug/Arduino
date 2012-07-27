#include <Wire.h>
#include "ADXL345.h"

template<class T>
inline HardwareSerial &operator <<(HardwareSerial &stream, T arg) {
  stream.print(arg);
  return stream;
}
const char endl[] = "\r\n";

const int I2C_ADXL345_ADDR = 0x53;
const int I2C_HMC5843_ADDR = 0x1E;

ADXL345 adxl(I2C_ADXL345_ADDR);

float gravity;
long velx;
long pox;

inline float norm(int x, int y, int z) {
  return sqrt((long)x*x+(long)y*y+(long)z*z);
}

int px, py, pz;
long posx=0, posy=0, posz=0;

long lastmillis;

void setup() {
  Serial.begin(57600);
  Serial.flush();
  Wire.begin();
  adxl.init();

  adxl.calibrate(12);
  gravity = norm(adxl.ax0,adxl.ay0,adxl.az0);
  Serial << adxl.ax0 << ", " << adxl.ay0 << ", " << adxl.az0 << " gravity " << gravity << endl;
  velx = 0;
  pox = 0;
}


long lastupdate;

void loop() {
  int avx, avy, avz;
  int arx, ary, arz;

  if ( millis() - lastupdate > 24 ) {
    adxl.read();
    lastupdate = millis();
    adxl.record();
    adxl.getAverage(12, avx, avy, avz);
    adxl.getAverage(3,arx, ary, arz);
    //  Serial << arx << ", " << ary << "; " << avx << ", " << avy << endl;
    if ( abs(arx-avx)+abs(ary-avy)+abs(arz-avz) < 8 ) {
      adxl.ax0 = avx;
      adxl.ay0 = avy;
      adxl.az0 = avz;
      velx /= 2;
    }
    if ( norm(arx-adxl.ax0, ary-adxl.ay0,0) > 5 ) {
      velx += arx - adxl.ax0;
      Serial << arx << ", " << ary << ", " << ": " << velx << endl;
    }
  }
}






