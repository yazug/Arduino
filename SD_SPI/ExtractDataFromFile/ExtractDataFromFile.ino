/*
  SD card file dump
 
 This example shows how to read a file from the SD card using the
 SD library and send it over the serial port.
 	
 * SD card must be attached to default SPI bus
 * with specific CS pin.
 
 created  22 December 2010
 by Limor Fried
 modified 9 Apr 2012
 by Tom Igoe
 
 This example code is in the public domain. 	 
 */

#include <SPI.h>
#include "SD_SPI.h"

#include "TextStream.h"

// On the Ethernet Shield, CS is pin 4. 
const int chipSelect = 4;

SDClass SD(chipSelect);

const char * fnames[] = { 
  "netintf.txt", "schedule.txt", "keys.txt" };

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.print("Initializing SD card... ");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  SPI.begin();  //pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin()) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(fnames[1]);
  char buf[128];

  // if the file is available, write to it:
  if (dataFile) {
    TextStream rw(dataFile);
    int istart, len;
    boolean skiphereafter;
    while (rw.available()) {
      skiphereafter = false;
      //      rw.peek();
      // seems to wait for around 35 millisec for the first char ...
      if ( rw.readLine(buf, 127, 50) ) {
        for(int itk = 0; (len = rw.ithToken(buf, itk, istart)) != 0; itk++) {
          if ( buf[istart] == '#' ) {
            skiphereafter = true;
            break;
          }
          Serial.write((byte*)buf+istart, len);
          Serial.print(" ");
        }
        Serial.println();
      }
    }
    dataFile.close();
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.print("error opening ");
    Serial.println(fnames[1]);
  } 
}

void loop()
{
}








