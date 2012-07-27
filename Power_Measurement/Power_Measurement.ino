#include <SPI.h>
#include "DS3234.h"
#include "SD.h"
#include <SoftwareSerial.h>
#include "Monitor.h"

DS3234 rtc(9);
int adc(const byte);

SoftwareSerial sport(2,3);
Monitor mon(Serial);

const int SD_CS = 10;
char logfile[32];

void setup() {
  mon.begin(19200);

  SPI.begin();
  rtc.begin();

  char buf[32];
  rtc.update();
  sprintf(logfile, "20%06lx.log", rtc.cal);
  mon << logfile << mon.endl;
  if (!SD.begin(SD_CS)) {
    mon << " ...card failed, or not present";
    // don't do anything more:
    return;
  }
  mon << " card initialized." << mon.endl;
  mon << "Hi." << mon.endl;

  mon.begin(19200);
}

void loop() {

  long swatch = micros();
  float vrms = getPower(1,0);
  swatch = micros() - swatch;

  rtc.update();
  mon << "date: ";
  mon.print(rtc.cal,HEX);
  mon << " time: ";
  mon.print(rtc.time, HEX);
  mon << " Vrms: " << vrms << mon.endl;

  File dataFile;
  if ( (dataFile = SD.open(logfile, FILE_WRITE)) ) {
    //    dataFile.print("Vrms = ");
    dataFile.print(rtc.cal, HEX);
    dataFile.print(",");
    dataFile.print(rtc.time, HEX);
    dataFile.print(",");
    dataFile.print(vrms, 2);
    dataFile.println();
    dataFile.close();
  }  
  // if the file isn't open, pop up an error:
  else {
    mon.println("error opening");
  } 
  delay(5000);
}

float getPower(byte pch, byte gndch) {
  long v0sum = 0;
  for (int i = 0; i < 64; i++) {
    v0sum += adc(gndch);
    delayMicroseconds(21);
  }
  int gndavr = v0sum / 64;
  v0sum = 0;
  for (int i = 0; i < 64; i++) {
    int d = adc(gndch) - gndavr;
    v0sum += d*d;
    delayMicroseconds(267);
  }
  v0sum = sqrt(v0sum/64/63);

  unsigned int diff;
  long sum = 0;
  for (int i = 0; i < 256; i++) {
    diff = adc(pch) - gndavr; 
    sum += diff*diff;
    delayMicroseconds(21);
  }

  return sqrt(sum/256);

}

int adc(const byte pin = 0) {
  byte low;
  word high;

  ADMUX = (DEFAULT << 6) | (pin & 0x07);

  // start the conversion
  ADCSRA |= (1<<ADSC);

  // ADSC is cleared when the conversion finishes
  while ( ADCSRA & (1<< ADSC));

  // we have to read ADCL first; doing so locks both ADCL
  // and ADCH until ADCH is read.  reading ADCL second would
  // cause the results of each conversion to be discarded,
  // as ADCL and ADCH would be locked when it completed.
  low  = ADCL;
  high = ADCH;

  return (high << 8) | low;
}









