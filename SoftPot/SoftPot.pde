#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <SerLCD.h>

SerLCD sLCD(8);
long lastupdate;

void setup() {
  sLCD.begin(16,2);
  sLCD.print(max(0, 34*((long)analogRead(0)-16)>>7));
  lastupdate = millis();
}

void loop() {
  if ( millis() - lastupdate > 500 || lastupdate > millis()){
    sLCD.setCursor(0,1);
//    if (digitalRead(14)) {
  //    sLCD.print("HIGH");
//    } else {
  //    sLCD.print("LOW");
 //   }
    //
  sLCD.setCursor(0,0);
  sLCD.print(max(0, 34*((long)analogRead(0)-16)>>7));
  sLCD.print("   ");
  lastupdate = millis();
  }
}

