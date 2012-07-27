/*
  SD card datalogger
 
 This example shows how to log data from three analog sensors 
 to an SD card using the SD library.
 	
 The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 
 created  24 Nov 2010
 updated 2 Dec 2010
 by Tom Igoe
 
 This example code is in the public domain.
 	 
 */

#include <SD.h>
#include <SoftwareSerial.h>

// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
const int SD_CS = 4;
const int CS = 10;
long tstamp;


SoftwareSerial ser_imu(6,7);
SoftwareSerial ser_gps(2,3);
boolean imu_reading;
boolean gps_reading;

void setup()
{
  Serial.begin(19200);
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(CS, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CS)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  pinMode(2, INPUT);
  pinMode(3, OUTPUT);
  ser_gps.begin(4800);
  gps_reading = false;
  
  pinMode(6, INPUT);
  pinMode(7, OUTPUT);
  ser_imu.begin(19200);
  imu_reading = false;
  
  tstamp = millis();
//  ser_gps.listen();
}

void loop()
{
  // make a string for assembling the data to log:
  String dataString = "";

  if ( !imu_reading && millis() > tstamp + 1000 ) {
    tstamp = millis();
    // read three sensors and append to the string:
    
    ser_imu.print('A');
  //  ser_imu.listen();
  }
  
  char tmp[64];
  /*
  if ( ser_imu.isListening() && read_serial(ser_imu, '!', '$', imu_reading, tmp) ) {
    Serial.println(tmp);
    ser_gps.listen();
  } else*/ 
  if ( read_serial(ser_gps, '$', (char) 0x0a, gps_reading, tmp) ) {
    Serial.println(tmp);
  }
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  //  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  //if (true /*dataFile*/) {
  //    dataFile.println(dataString);
  //    dataFile.close();
  // print to the serial port too:
  // Serial.println(dataString);
  //}  
  // if the file isn't open, pop up an error:
  //else {
  //  Serial.println("error opening datalog.txt");
  //} 
  //}
}

boolean read_serial(SoftwareSerial & sport, char head, char terminal, 
                    boolean & readingStatus, char * output) {
  boolean completed = false;
  char c;
  while ( sport.available() ) {
    c = sport.read();
    if ( c == head ) {
      readingStatus = true;
    }
    if ( readingStatus ) {
      *output++ = c;
      if ( c == terminal ) {
        readingStatus = false;
        completed = true;
        *output++ = (char) 0;
      }
    }
  }
  return completed;
}


