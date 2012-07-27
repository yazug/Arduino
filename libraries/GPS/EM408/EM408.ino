#include <SoftwareSerial.h>
#include "GPS.h"

#include <Wire.h>
#include "LCD.h"
#include "LCD_ST7032i.h"

#include <SPI.h>
#include <MySD.h>

#include <PrintUtilities.h>
using namespace prt;

LCD_ST7032i lcd = LCD_ST7032i();
//xLCD lcd(0x07, 6,5,4,3,2,1,0, 7);
//xLCD lcd(0x00, 1, 0xff, 2, 3, 4, 5, 6, 7);

const int SD_CS = 4;
enum {
  NO_CARD = 0,
  VOLUME_MOUNTED,
  FILE_OPENDED
} 
sd_state;

SoftwareSerial softser(2,3);
GPS gps(softser);
long last;
char logfilename[] = "00000000.txt";

void setup() {
  softser.begin(4800);
  Serial.begin(19200);
  SPI.begin(); 
  sd_state = NO_CARD; 
  Wire.begin();

  lcd.begin();
  lcd.clear();
  lcd.backlightOn();
  lcd.print("Start.");
  Serial << "Start." << endl;


  lcd.clear();
  for (int i = 0; i < 5; i++) {
    if ( SD.begin(SD_CS) ) {
      sd_state = VOLUME_MOUNTED;
      break;
    }
    lcd.clear();
    lcd.print("Waiting SD init.");
    delay(1000);
  }

  while ( gps.caldate == 0 ) {
    lcd.clear();
    lcd.print("Waiting update.");
    while ( !gps.update( GPS::RMC) );
    Serial << gps.caldate << prt::endl;
    delay(500);
  }
  lcd.clear(); 
  printOn(gps.caldate, logfilename);
  Serial << logfilename << prt::endl;
  lcd.print("Ok.");
  last = millis();
}

void loop() {
  long tval;
  char tmp[8];
  if ( gps.update(GPS::GGA | GPS::RMC) ) {
    //Serial << "Type = " << gps.sentenceType(); 
    // << ", " << gps.sentence() << Monitor::endl;
    Serial << " Date = " << gps.caldate << ", Time = " << gps.time << endl;
    Serial << "Lat = " << gps.latitude << ", Longi = " << gps.longitude 
      << ", Alt = " << gps.altitude << ", G. speed = " << gps.grndspeed 
      << ", Track angle = " << gps.trackangle;
    Serial << endl;

    //
    tval = gps.time/100;
    lcd.setCursor(0,0);
    // lcd.print("JST ");
    tval += 90000L;
    tval %= 240000L;
    //    Serial << printOn(tval,"000000") << endl;
    lcd.print(printOn(tval,"00:00:00"));
    //
    lcd.setCursor(0,1);
    lcd.print(printOn(gps.latitude,"+00.0000"));
    lcd.setCursor(8,1);
    lcd.print(printOn(gps.longitude,"+00.0000"));
    //
    lcd.setCursor(9, 0);
    lcd.print(gps.grndspeed,2);
    /*
    if ( gps.altitude != 0 ) {
     lcd.print(gps.altitude,1);
     Serial.print(gps.altitude,1);
     Serial.println(" m");
     } 
     else if ( gps.groundspeed != 0 ) { */
    //    }
    //
    if ( sd_state == NO_CARD ) {
      if ( SD.begin(SD_CS) ) {
        sd_state = VOLUME_MOUNTED;
      }
    }
    if ( sd_state == VOLUME_MOUNTED ) {
      File dataFile = SD.open(logfilename, FILE_WRITE);
      if ( dataFile ) {
        dataFile << gps.time << ", " << gps.caldate << ", ";
        dataFile << gps.latitude << ", ";
        dataFile << gps.longitude << ", ";
        dataFile << gps.dilution << ", ";
        dataFile << gps.altitude << ", ";
        dataFile << gps.grndspeed << ", " << gps.trackangle << endl;
        dataFile.close();
      } 
      else {
        sd_state = NO_CARD;
        Serial << "SD write failed. " << logfilename << prt::endl;
        lcd.setCursor(0,0);
        lcd.print("SD write failed.");
      }
    } 
    else {
      Serial << "No card." << prt::endl;
      lcd.setCursor(0,0);
      lcd.print("No card.");
      sd_state = NO_CARD;
    }
    gps.clear();
  }
}







