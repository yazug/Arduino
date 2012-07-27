/*
#include <WProgram.h>
#include "StringStream.h"
#include "Sensirion.h"
#include "Wire.h"
#include "DS1307.h"
*/
#include "matrix8x8.h"

#include "SPI.h"
#include "TimerCounter.h"
#include "IOExpander.h"
#include "HC595iox.h"

#define S_DATA_PIN  5
#define S_SCK_PIN   4

float temp;
float humi;
float dewp;

//Sensirion tempSensor = Sensirion(S_DATA_PIN, S_SCK_PIN);

char * DAYS[] = {
  "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

char * MONTHS[] = {
  "Dec", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

const int latchPin = 10;   //Pin connected to ST_CP of 74HC595
const int clockPin = 13;   //Pin connected to SH_CP of 74HC595
const int dataPin  = 11;   //Pin connected to DS of 74HC595

HC595iox xpander(latchPin);

void setup() {
  //set pins to output because they are addressed in the main loop
//  pinMode(latchPin, OUTPUT);
//  pinMode(clockPin, OUTPUT);
//  pinMode(dataPin, OUTPUT);  
  SPI.begin();
  xpander.begin();
//  xpander.setBitOrder(LSBFIRST);

//  TimerCounter2::prescaler(2);
  analogWrite(3,40);
  
//  Wire.begin();

//  Serial.begin(9600);
}

int n = 0;
unsigned char buffer[8];
struct StringBuffer {
  const unsigned char * string;
  const unsigned int length;

  StringBuffer(unsigned char buf[], int bufsize) : string(buf), length(bufsize) {}
} sbuf(buffer, 8);
//= "If thou beest he; But O how fall'n! how chang'd From him, who in the happy Realms of Light Cloth'd with transcendent brightness didst out-shine ";
long elapsed = millis();
int intens = 0;
//StringStream sbuf(message, 64);

boolean busy = false;

void loop() {
  word wval, wval2;
  int i, s;
  int fw = sizeof(font[0])+1;
  unsigned long t;

  if (!busy) {
    busy = true;
  sprintf((char*)sbuf.string,"%d", millis()/1000);
/*
    t = RTC.time(true);
    tempSensor.measure(&temp, &humi, &dewp);
    
    sbuf.flush();
    sbuf.print("  ");
    sbuf.print(temp+0.05, 1);
    sbuf.print("`C ");
    sbuf.print(humi+0.05, 1);
    sbuf.print("% ");
    
    t /= 60;  // waste secs.
    sbuf.print(t/600);
    sbuf.print(t/60%10);
    sbuf.print(":");
    t %= 60;
    sbuf.print(t/10);
    sbuf.print(t%10);
    sbuf.print(" ");
    t = RTC.date(true);
    sbuf.print( DAYS[t%10] );
    sbuf.print(",");
    t /= 10;
    sbuf.print( t%100 );
    sbuf.print( " " );
    sbuf.print( MONTHS[t/100%100] );
    sbuf.print( " " );
    sbuf.print( t/10000 );
    sbuf.print( " " );
*/
  }

  //count up routine
  for (int r = 0; r < 8; r++) {
    wval = allColumnOff;
    wval |= rowBV[r];
    /*
    wval2 = allColumnOff;
    wval2 |= rowBV[r];
    */
    for (int c = 0; c < 8; c++) {
      if ( (n+c)%fw < fw-1 )
        if ( (font[sbuf.string[(n+c)/fw % sbuf.length]-0x20][(n+c)%fw]>>r) & 1 )
          wval &= ~colBV[c];
    }
    /*
    for (int c = 8; c < 16; c++) {
      if ( (n+c)%fw < fw-1 )
        if ( (font[sbuf.string[(n+c)/fw % sbuf.length]-0x20][(n+c)%fw]>>r) & 1 )
          wval2 &= ~colBV[c];
    }
    */
    byte data[] = { highByte(wval), lowByte(wval)}; //, highByte(wval), lowByte(wval) };
    xpander.write(data, 2);
    delayMicroseconds(331);
  }
  //

  if ( millis() - elapsed > 133 ) {
    n++;
    if ( n > sbuf.length * fw ) {
      n = 0;
      busy = false;
    }
    elapsed = millis();    
  }

} 

