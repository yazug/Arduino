#include <Pololu3pi.h>
#include <PololuQTRSensors.h>
#include <OrangutanMotors.h>
#include <OrangutanPushbuttons.h>
#include <OrangutanAnalog.h>

#include "Monitor.h"

Monitor monitor(Serial);
long watch;
int mright, mleft;
const int front_th = 100;
boolean toggle;

void setup() {
  DDRC &= 0b00011111;
  PORTC &= 0b00011111;

  monitor.begin(9600);
  monitor << "Hello." << Monitor::endl;
  monitor << "Battery " << read_battery_millivolts() << " mV" << Monitor::endl;

  mleft = 0;
  mright = 0;
  toggle = false;
}

void loop() {
  int acc;
  if ( millis() > watch + 100 ) {
    int adc6 = read(6, 3);
    int adc5 = read(5, 3);
    watch = millis();
    monitor << adc5 << ", " << adc6 << Monitor::endl;
    if ( adc5 > 500 ) {
      acc = -32;
    } 
    else if ( adc5 > 384 ) {
      if ( abs(mleft) < 100 or abs(mright) < 100 ) {
        acc = -48;
      } 
      else {
        acc = -24;
      }
    } 
    else if ( adc5 < 384 ) {
      if ( abs(mleft) < 100 or abs(mright) < 100 ) {
        acc = 64;
      } 
      else {
        acc = 16;
      }
    }
    mleft = max(-96, mleft + acc);
    mright = max(-64, mright + acc);
  }
  if ( button_is_pressed(BUTTON_B) ) {
    long tmp = millis();
    while ( button_is_pressed(BUTTON_B) );
    if ( millis() > tmp + 50 )
      toggle = !toggle;
  }
  if ( !toggle ) {
    if ( mleft > 0 or mright > 0 ) {
      mleft = 0;
      mright = 0;
    }
  }
  set_motors(mleft, mright);
}

int read(int ch, int time) {
  int result = analog_read(ch);
  for(int i = 0; i < time; i++) {
    delay(2);
    result += analog_read(ch);
  }
  return result/time;
}



