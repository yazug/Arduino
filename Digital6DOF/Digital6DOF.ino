#include <SoftwareSerial.h>
#include <Wire.h>
#include <SD.h>

#include "ADXL345.h"
#include "ITG3200.h"
#include "Monitor.h"

ADXL345 accel;

struct Gyro {
  ITG3200 gyro;
  float gx0, gy0, gz0;
  float angle;
  const static float INTERVAL = 33;
  long thisInterval;
  long lastUpdate;

  void begin() {
    gx0 = 0;
    gy0 = 0;
    gz0 = 0;
    angle = 0;
    gyro.begin();
  }

  void setOffset() {
    gyro.update();
    gx0 = gyro.getX();
    gy0 = gyro.getY();
    gz0 = gyro.getZ();
    for(int i = 0; i < 35; i++) {
      delay(INTERVAL);
      gyro.update();
      gx0 = (gx0 + gyro.getX())/2;
      gy0 = (gy0 + gyro.getY())/2;
      gz0 = (gz0 + gyro.getZ())/2;
    }
    lastUpdate = millis();
  }

  void update() { 
    gyro.update();
    thisInterval = millis() - lastUpdate;
    lastUpdate = millis();
    angle += (rotZ() * thisInterval) / 1000 /* scale = 0.0366 */;
  }

  float rotX() {
    return gyro.getX() - gx0;
  }

  float rotY() {
    return gyro.getY() - gy0;
  }
  
  float rotZ() {
    return gyro.getZ() - gz0;
  }
} 
gyro;

// Generic template
template<class T>
inline Print &operator <<(Print &stream, T arg) {
  stream.print(arg);
  return stream;
}

const int CS_MPU = 10;
const int CS_SD = 4;
boolean SD_active;

void setup() {
  Serial.begin(19200);

  pinMode(CS_MPU, OUTPUT); // SPI mode
  pinMode(CS_SD, OUTPUT);  // CS_SD

  Wire.begin();
  accel.begin();
  gyro.begin();
  gyro.setOffset();

  if ( SD.begin(CS_SD) ) {
    SD_active = true;
    File dataFile = SD.open("gyro.txt", FILE_WRITE);
    if (dataFile) {
      dataFile  << "Offset = " << gyro.gx0 << ", " << gyro.gy0 << ", " << gyro.gz0 << "\r\n";
      dataFile.close();
      Serial  << "Offset = " << gyro.gx0 << ", " << gyro.gy0 << ", " << gyro.gz0 << "\r\n";
    }
  } 
  else {
    SD_active = false;
    Serial.println("SD Card init failed, or not present.");
    // don't do anything more:
    pinMode(13, OUTPUT);
    digitalWrite(CS_MPU, LOW);
  }
  //Serial.println("Start.");
  digitalWrite(CS_MPU, HIGH);
  delay(1000);
  digitalWrite(CS_MPU, LOW);
}

void loop() {
  File datafile;

  if ( millis() > gyro.lastUpdate + gyro.INTERVAL ) {
    digitalWrite(CS_MPU, HIGH);
    accel.update();
    gyro.update();

    if ( SD_active && (datafile = SD.open("gyro.txt", FILE_WRITE)) ) {
      datafile  << millis() << ", " << gyro.thisInterval << ", " << accel.getX() << ", " << accel.getY() << ", " << accel.getZ() << ", "
        << gyro.rotX() << ", " << gyro.rotY() << ", " << gyro.rotZ() 
        << ", " << gyro.angle << ", " << "\r\n";
      datafile.close();
      // print to the serial port too:
    }
    // if the file isn't open, pop up an error:
    else {
      if ( Serial.available() ) {
        switch ( Serial.read() ) {
        case 'A':
        case 'a':
          Serial  << millis() << ", " << gyro.thisInterval << ", " << accel.getX() << ", " << accel.getY() << ", " << accel.getZ() << "\r\n";
          break;
        case 'G':
        case 'g':
          Serial  << millis() << ", " << gyro.thisInterval << ", "<< gyro.rotX() << ", "<< gyro.rotY() << ", "<< gyro.rotZ() << ", " << gyro.angle << "\r\n";
          break;
        }
      }
    }
    digitalWrite(CS_MPU, LOW);
  }

}



