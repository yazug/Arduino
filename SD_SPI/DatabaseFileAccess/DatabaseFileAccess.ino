#include <SPI.h>
#include "SD_SPI.h"
//#include "Des.h"
#include "TextStream.h"

#include "RecordBuffer.h"
// On the Ethernet Shield, CS is pin 4. 
const int chipSelect = 4;
SDClass SD(chipSelect);

const char * dbfname = "keys.dbn";

const byte	uni_key[8]  = { 
  0x7F, 0x48, 0xE5, 0xEC, 0x12, 0x0F, 0xE1, 0x9E}; 	/* defoult net_key */
const byte	sys_key[8]  = { 
  0x5C, 0x78, 0x51, 0xCB, 0x6A, 0xB5, 0x4E, 0x5E};
//Des codec(uni_key);

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial);
  delay(1000);

  Serial.print("Initializing SD card... ");

  SPI.begin();  //pinMode(10, OUTPUT);

  SD.begin() || (Serial.println("Card initialization failed... may be not present") && halt() );
  Serial.println("card initialized.");

  File dbfile = SD.open(dbfname);
  if ( !dbfile ) Serial.println("Failed to open db file.");

 char buf[512];
  RecordBuffer record;

long swatch = millis();
  if (dbfile) {
    int cnt = 0;
    while (dbfile.available()) {
      dbfile.read(record.rawbytes, 32);
      Serial.print(cnt);
      Serial.print(" ");
      record.printOn(Serial);
      cnt++;
    }
    dbfile.close();
    Serial.println(millis() - swatch);
    Serial.println("finished.");
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.print("error opening ");
    Serial.println(dbfname);
  } 
}

void loop()
{
}

boolean halt() {
  for(;;);
  return true;
}









