#include <WProgram.h>

struct ADXL345 {
  static const int SUCCESS = true;
  static const int FAILURE = false;
  static const int HISTORY_SIZE = 16;

  byte i2c_addr;
  int x, y, z;
  int ax0, ay0, az0;
  int hindex;
  struct {
    int x, y, z;
  } hist[HISTORY_SIZE];
  
  ADXL345(byte addr) {
    i2c_addr = addr;
  }

  void init() {
    Wire.beginTransmission(i2c_addr);
    Wire.send(0x2D);  // power register
    Wire.send(0x08);  // measurement mode
    Wire.endTransmission();
    delay(5);
    Wire.beginTransmission(i2c_addr);
    Wire.send(0x31);  // Data format register
    Wire.send(0x08);  // set to full resolution
    Wire.endTransmission();
    delay(5);	
    // Because our main loop runs at 50Hz we adjust the output data rate to 50Hz (25Hz bandwidth)
    Wire.beginTransmission(i2c_addr);
    Wire.send(0x2C);  // Rate
    Wire.send(0x09);  // set to 50Hz, normal operation
    Wire.endTransmission();
    delay(5);
    hindex = 0;
  }

  byte read() {
    int i = 0;
    byte buff[6];

    Wire.beginTransmission(i2c_addr); 
    Wire.send(0x32);        //sends address to read from
    Wire.endTransmission(); //end transmission

      Wire.beginTransmission(i2c_addr); //start transmission to device
    Wire.requestFrom(i2c_addr, (byte) 6);    // request 6 bytes from device

    for (i = 0; Wire.available(); i++) {  // ((Wire.available())&&(i<6))
      if (i > 6) break;
      buff[i] = Wire.receive();  // receive one byte
    }
    Wire.endTransmission(); //end transmission
    if (i==6) { // all the bytes read
      x = (int(buff[1]) << 8) | buff[0];
      y = (int(buff[3]) << 8) | buff[2];
      z = (int(buff[5]) << 8) | buff[4];
      return SUCCESS;
    }
    return FAILURE;
  }
  
  void record() {
    hindex = (hindex+1) % HISTORY_SIZE;
    hist[hindex].x = x;
    hist[hindex].y = y;
    hist[hindex].z = z;
  }
  
  void getAverage(int window, int & avx, int & avy, int &avz) {
    avx = 0;
    avy = 0;
    avz = 0;
    int tindex;
    for(int i = 0, cnt = 0; i < window ; i++) {
      tindex = (hindex - i + HISTORY_SIZE) % HISTORY_SIZE;
      avx += hist[tindex].x;
      avy += hist[tindex].y;
      avz += hist[tindex].z;
    }
    avx /= window;
    avy /= window;
    avz /= window;
  }

  void calibrate(int limit) {
    for ( int c = 0; c < limit; c++) {
      read();
      record();
      delay(25);
    }
    getAverage(min(HISTORY_SIZE, limit), ax0, ay0, az0);
  }
};



