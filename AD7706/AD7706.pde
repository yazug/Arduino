#include <SPI.h>

const byte AD7706_DRDY = 8;
const byte AD7706_CS = 10;

void setup() {
  pinMode(AD7706_DRDY, INPUT);
  pinMode(AD7706_CS, OUTPUT);
  digitalWrite(AD7706_CS, HIGH);
  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  
  Serial.begin(38400);
  Serial.println(digitalRead(AD7706_DRDY));  
  delay(500);
}

void loop() {
  digitalWrite(AD7706_CS, LOW);
  SPI.transfer((byte)0x20); // Communication, select Clock Register
  SPI.transfer((byte)0x04); // 1MHz w/ divide
  SPI.transfer((byte)0x10); // Communication, select Setup Register
                      // w/ CH1:0, CH0:0
  SPI.transfer((byte)0x04);
//  while (digitalRead(AD7706_DRDY));
  digitalWrite(AD7706_CS, HIGH);

  Serial.println(digitalRead(AD7706_DRDY));  
  delay(1000);
}

