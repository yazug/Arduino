#include <Wire.h>
#include "MCP23009.h"

#include <LiquidCrystal.h>
#include "xLCD.h"

// initialize the library with the numbers of the interface pins
xLCD lcd(0x00, 6,5,4,3,2,1,0, 7);
/*
#include <SerLCD.h>
#include <SoftwareSerial.h>
*/
byte plusminus[8] = {
  0b00100,
  0b00100,
  0b11111,
  0b00100,
  0b00100,
  0b00000,
  0b11111,
  0b00000};

const long uSEC_SAMPLING = 16666;
const float mV_PER_ADCOUNT= float(5000)/1024;
const long mSEC_INTERVAL = 1000;
int prevValue;

//SerLCD slcd(2);

void setup() {
  Wire.begin();
  // set up the LCD's number of rows and columns: 
  lcd.begin(16, 2);
  //slcd.begin(9200, 16,2);
//  Serial.begin(19200);
//  Serial.println("Hello.");
  //lcd.backlightOn();
  lcd.createChar(7, plusminus);
  lcd.clear();
  lcd.print(7, BYTE);
  lcd.print(" Hello.");
  delay(1000);
  lcd.clear();
  
  prevValue = 0;
}

void loop() {
  int val, valmax, valmin, ref;
  int count;
  long sum;
  long swatch;
  
  char tmp[24];
  
  swatch = micros();
  val = analogRead(0);
  valmax = val;
  valmin = val;
  sum = val;
  count = 1;
  ref = analogRead(1);
  while (count < 331) {
    delayMicroseconds(667);
    val = analogRead(0);
    sum += val;
    valmin = min(val, valmin);
    valmax = max(val, valmax);
    count++;
    //
    ref += analogRead(1);
    ref /= 2;
  }
  
  lcd.setCursor(0,0);
  sprintf(tmp, "%4d", count);
  lcd.print(tmp);
  lcd.print(" Ref: ");
  lcd.print( ref * 5.1/1024, 3);
//  slcd.print(count);
//  slcd.print(" Ref=");
//  slcd.print(ref);
  lcd.setCursor(0,1);
//  Serial.println(count);
  val = sum/count;
  val = (val + prevValue)/2;
  if ( abs(prevValue - val) > 0 ) {
    lcd.print("*");
  } else {
//    Serial.print("   ");
    lcd.print(" ");
  }
  //val = (val + prevValue)/2;
  lcd.print(val);
  lcd.print(' ');
  lcd.print(7, BYTE);
  lcd.print(' ');
  lcd.print((valmax-valmin)/2);
//  Serial.print(val);
//  Serial.print(" +/-");
//  Serial.println((valmax-valmin)/2);
  lcd.print(" ");
  prevValue = val;
  
  delay(mSEC_INTERVAL);
}

