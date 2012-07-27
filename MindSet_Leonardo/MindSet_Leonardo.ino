#include "MindSet.h"
#include "Monitor.h"
#include <Servo.h>

MindSet m3band(Serial1);

Servo microServo;
int servoPos;

long lastMessaged;
const int pilotLED = 13;
const int LED_yellow = 10;
const int LED_blue = 9;

void setup() {
  Serial.begin(38400);
  Serial1.begin(57600);
  pinMode(pilotLED, OUTPUT);
  pinMode(LED_yellow, OUTPUT);
  pinMode(LED_blue, OUTPUT);

  microServo.attach(11);
  servoPos = 0;

  Serial1.write(0xc2);
  lastMessaged = millis();
}

void loop() {
  boolean result = m3band.getPacket();
  if ( result ) {
    m3band.parsePayload();
    if ( m3band.poorSignalQuality() > 20 )
      blink();
    else
      digitalWrite(pilotLED, HIGH);
    /*
  if ( m3band.poorSignalQuality() > 20 && millis() > lastMessaged + 1000) {
     Serial << "Poor." << Monitor::endl;
     lastMessaged = millis();
     }  
     */
    if ( m3band.EEG_updated ) {
      //      Serial << "EEG." << Monitor::endl;
      Serial << "att: " << m3band.attention << ", med: " << m3band.meditation << Monitor::endl;
      Serial << "Delta: " << dlog2(m3band.EEG[m3band.DELTA])
             << ", Theta: " << dlog2(m3band.EEG[m3band.THETA])
             << ", Alpha: " << dlog2(m3band.EEG[m3band.LOW_ALPHA]) 
             << ", " << dlog2(m3band.EEG[m3band.HIGH_ALPHA]) 
             << ", Beta: " << dlog2(m3band.EEG[m3band.LOW_BETA]) 
             << ", " << dlog2(m3band.EEG[m3band.HIGH_ALPHA]) 
             << ", Gamma: " << dlog2(m3band.EEG[m3band.LOW_GAMMA]) 
             << ",  " << dlog2(m3band.EEG[m3band.MID_GAMMA]) 
             << Monitor::endl;
    }
  } 
  else {
    blink();
  }
  perform_lights();
}

int dlog2(long x) {
  int n;
  for (n = 0; x > 1; n++) {
    x = x/2;
  }
  return n;
}


void perform_lights() {
  static int yellow = 100, blue = 0;
  static int attavr = 100, medavr = 0;
  static long swatch = millis();
  //
  attavr = (attavr + m3band.attention) / 2;
  medavr = (medavr + m3band.meditation) / 2;

  if ( yellow < 50 && attavr > 60 ) {
    yellow = 100;
  } 
  else if ( yellow >= 50 && attavr < 40 ) {
    yellow = 0;
  }
  if ( blue < 50 && medavr > 60 ) {
    blue = 100;
  } 
  else if ( blue >= 50 && medavr < 40 ) {
    blue = 0;
  }

  if ( blue > 50 ) {
    digitalWrite(LED_blue, HIGH);
  } 
  else {
    digitalWrite(LED_blue, LOW);
  }
  if ( yellow > 50 ) {
    digitalWrite(LED_yellow, HIGH);
  } 
  else {
    digitalWrite(LED_yellow, LOW);
  }

  if ( millis() > swatch + 37 ) {
    swatch = millis();
    //
    int tmp;
    if ( yellow > 50 && blue < 50 ) {
      tmp = (servoPos + 100)/2;
    } else if ( yellow < 50 && blue < 50 ) {
      tmp = (servoPos + 0)/2;
    } else if ( blue > 50 && yellow > 50 ) {
      tmp = (servoPos + 30 + 16*sin( 0.0628*(millis()/100%100) ) ) / 2;
    } else if ( blue > 50 && yellow < 50 ) {
      tmp = (servoPos + 8 + 8*sin( 0.0628*(millis()/100%100) ) ) / 2;
    }
    if ( tmp != servoPos ) 
      microServo.write( 100 - (servoPos = tmp));
  }
}

void blink() {
  static boolean lit = false;
  static long swatch = millis();

  if ( millis() > swatch + 500 ) {
    lit = !lit;
    digitalWrite(pilotLED, lit == true);
    swatch = millis();
  }
}





