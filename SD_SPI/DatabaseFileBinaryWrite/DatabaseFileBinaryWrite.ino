#include <SPI.h>
#include "SD_SPI.h"
#include "Des.h"
#include "TextStream.h"
#include "SPISRAM.h"


#include "ICCardKey.h"
// On the Ethernet Shield, CS is pin 4. 
const int SD_chipSelect = 4;
SDClass SD(SD_chipSelect);

const char * datafname = "keys.txt";
const char * dbfname = "keys.dbn";

const byte	uni_key[8]  = { 
  0x7F, 0x48, 0xE5, 0xEC, 0x12, 0x0F, 0xE1, 0x9E}; 	/* defoult net_key */
const byte	sys_key[8]  = { 
  0x5C, 0x78, 0x51, 0xCB, 0x6A, 0xB5, 0x4E, 0x5E};
//Des codec(uni_key);

const int SRAM_CS = 8;
SPISRAM sram(SRAM_CS, SPISRAM::BUS_MBits); // CS pin

void setup()
{
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  //
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial);
  //delay(1000);

  Serial.print("Initializing SD card... ");

  SPI.begin();  //pinMode(10, OUTPUT);

  SD.begin() || (Serial.println("Card failed, or not present") && halt() );
  Serial.println("card initialized.");
  sram.begin();
  
  File datafile = SD.open(datafname);
  if ( !datafile ) Serial.println("Failed to open data file.");
  //  File dbfile = SD.open(dbfname, FILE_WRITE);
  //  if ( !dbfile ) Serial.println("Failed to open db file.");

  char buf[128];
  ICCardKey record;

  long swatch = millis();
  if (datafile) {
    buf[0] = 0;
    TextStream rw(datafile);
    int istart, llen;
    char * p;
    boolean reol;
    long cnt = 0;
    while (rw.available()) {
      reol = false;
      // seems to wait for around 35 millisec for the first char ...
      if ( rw.concateLine(buf, 127, 50) || (datafile.size() == datafile.position()) ) {
        if ( buf[0] == '#' || (llen = strlen(buf)) == 0 ) {
          buf[0] = 0;
          reol = true;
          continue;
        }

        for(int i = 0; i < llen; i++) {
          if ( buf[i] == '\t' || buf[i] == ',' ) {
            buf[i] = 0;
          }
        }
        record.init();
        for(int itn = 0, istart = 0; istart < llen ; istart++) {
          if ( strlen(buf+istart) > 0 ) {
            switch(itn) {
            case 0:
              record.type = atoi(buf+istart);
              break;
            case 1:
              record.DIVISION = buf[istart];
              break;
            case 2:
              strncpy(record.PID, buf+istart, 8);
              break;
            case 3:
              record.ISSUE = buf[istart];
              break;
            case 4:
              TextStream::scanBytes(buf+istart, record.NFCID, 8);
              break;
            case 5:
              record.goodthru = strtol(buf+istart, NULL, 16);
              break;
            }
          }
          //Serial.print(itn);
          //Serial.print(buf+istart);
          //Serial.print(", ");
          itn++;
          istart += strlen(buf+istart);
        }
        //        Serial.print(cnt);
        //        Serial.print(" ");
        //        record.printOn(Serial);
        //        Serial.println();
        //        if ( !dbfile.write(record.rawbytes, 32) )
        //          Serial.println("write failed!!!");
        sram.write(cnt * sizeof(ICCardKey), record.rawbytes, sizeof(ICCardKey));
        cnt++;
        buf[0] = 0;
      }
    }
    datafile.close();
    //    dbfile.close();
    Serial.println(millis() - swatch);
    Serial.println("Loading data finished.");
    Serial.println();
    Serial.println("Now reading data from Serial SRAM.");
    swatch = millis();
    for(long i = 0; i < cnt; i++) {
      sram.read(i*24, record.rawbytes, 24);
      if ( i < 5 || i > cnt - 4 ) {
        Serial.print(i);
        Serial.print(" ");
        record.printOn(Serial);
        if ( i == 4 ) 
          Serial.println("...");
      }
    }
    Serial.println(millis() - swatch);
    Serial.println("Dumping ");
    Serial.print(cnt);
    Serial.println(" x 24 bytes data finished.");
    Serial.println();

    swatch = millis();

    Serial.println("Search by type, division, pid and issue no.");
    int pos = bsearch(cnt, "\x10S825418541");
    Serial.print(millis() - swatch);
    Serial.println(" msec.");
    Serial.println(pos);
    sram.read(pos * 24, record.rawbytes, 24);
    record.printOn(Serial);

  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.print("error opening ");
    Serial.println(datafname);
  } 
}

void loop()
{
}

boolean halt() {
  for(;;);
  return true;
}

int bsearch(long limit, char id[]) {
  ICCardKey r;
  /*
  for(int i = 0; i < 11; i++) {
    Serial.print(id[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  */
  long ib = 0;
  long ie = limit;
  long ix;
  while (ie > ib) {
    ix = (ib+ie)/2;
    sram.read(ix * 24, r.rawbytes, 24);
    int diff = memcmp(r.rawbytes, id, 11);
/*
    r.printOn(Serial);
    Serial.print(ix);
    Serial.print(": ");
    for(int i = 0; i < 11; i++) {
      Serial.print(r.rawbytes[i], HEX);
      Serial.print(' ');
    }
    Serial.print(" :");
    Serial.print(diff);
    Serial.println();
    Serial.println();
    */
    if ( diff == 0 ) {
      return ix;
    }
    if ( diff > 0 ) {
      ie = ix;
    } 
    else {
      ib = ix + 1;
    }
  }
  return -1;
}


