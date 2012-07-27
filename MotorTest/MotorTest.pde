// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include "SerLCD.h"

int speed;
boolean safetySW = false;

SerLCD slcd(2,9600);

void setup() {
  slcd.begin(16,2);           // set up Serial library at 9600 bps
  slcd.println("Motor test!");

  // turn on motor
  speed = 0;
  analogWrite(9, speed);
  slcd.home();
  slcd.print("Speed = ");
  slcd.println(speed);
}

void loop() {
  int vol, i;
  vol = analogRead(0);
  slcd.setCursor(0,1);
  slcd.print(vol);
  if (!safetySW) {
    if (vol == 0)
      safetySW = ~safetySW;
    vol = speed;
    slcd.println("!");
  } else {
    slcd.println("");
  }
  
  speed += (vol - speed)/2 + (vol - speed)%2;
  analogWrite(9, speed>>2);

  slcd.home();
  slcd.print("Output = ");
  slcd.print((long)(speed>>2)*1000/255/10);
  slcd.print(".");
  slcd.print((long)(speed>>2)*1000/255%10);
  slcd.println("%");
}

