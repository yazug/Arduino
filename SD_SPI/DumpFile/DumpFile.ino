/*
  SD card file dump
 
 This example shows how to read a file from the SD card using the
 SD library and send it over the serial port.
 	
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 
 created  22 December 2010
 by Limor Fried
 modified 9 Apr 2012
 by Tom Igoe
 
 This example code is in the public domain. 	 
 */

#include <SPI.h>
#include <SD_SPI.h>

// On the Ethernet Shield, CS is pin 4. 
const int chipSelect = 4;

SDClass SD(chipSelect);

const char * fnames[] = { "netintf.txt", "shcedule.txt", "keys.txt" };

void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.print("Initializing SD card...");
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
  File dataFile = SD.open(fnames[0]);

  // if the file is available, write to it:
  if (dataFile) {
    
    while (dataFile.available()) {
      char c = dataFile.read();
      if ( c == '\x0a' || c == '\x0d') {
        Serial.println();
      } else {
        Serial.write(c);
      }
    }
    dataFile.close();
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.print("error opening ");
    Serial.println(fnames[0]);
  } 
}

void loop()
{
}

