/*
  Blink
 Turns on an LED on for one second, then off for one second, repeatedly.
 
 This example code is in the public domain.
 */
#include <ctype.h>

char code[][8] = {
  "313133", //!
  "131131",
  "",
  "1113113", //$
  "",
  "13111", //&
  "133331",
  "31331",
  "313313",
  "",
  "13131", //+
  "331133",
  "311113", // -
  "131313", // .
  "31131", 

  "33333", //0  
  "13333", // 1
  "11333",
  "11133",
  "11113",
  "11111", //5
  "31111",
  "33111",
  "33311",
  "33331", // 9
  "333111", //:
  "313131", // ;
  "", // <
  "31114", // =
  "",
  "113311", // ?
  "133131", // @
  "13", //A
  "3111",
  "3131",
  "311", //D
  "1",
  "1131",
  "331",
  "1111",
  "11",
  "1333",
  "313", //K
  "1311", //L
  "33",
  "31",
  "333",
  "1331",
  "3313",
  "131",
  "111",
  "3",
  "113",
  "1113",
  "133",
  "3113",
  "3133",
  "3311", // Z
  "",
  "",
  "",
  "",
  "113313" // _
};

const long one_dot = 30;
const int pin_LED = 13;
const int pin_IRLED = 3;
const int pin_Piezzo = 15;

void signal(int times) {
  digitalWrite(pin_LED, HIGH);
  digitalWrite(pin_IRLED, HIGH);
  tone(pin_Piezzo, 3100);
  delay(times * one_dot);
  digitalWrite(pin_LED, LOW);
  digitalWrite(pin_IRLED, LOW);
  noTone(pin_Piezzo);
  delay(one_dot);
}

void pause(int times) {
  noTone(pin_Piezzo);
  delay(times * one_dot);
}

char buffer[256];
int index;

void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(pin_LED, OUTPUT);
  pinMode(2,OUTPUT); // Serial out?
  digitalWrite(2, LOW);
  pinMode(pin_Piezzo, OUTPUT);
  pinMode(pin_IRLED, OUTPUT);



  strcpy(buffer,"In sixteen-twenty the mayflower sailed from plymouth to westward carrying the pilgrims to search for the new land.\r");

  Serial.begin(9600);
}

void loop() {
  int bix;

  if ( Serial.available() ) {
    for (bix = 0; Serial.available() && bix < 256 - 1; bix++) {
      buffer[bix] = (char) Serial.read();
      /*
      long start_wait = millis();
       while (!Serial.available()) {
       if ( millis() > start_wait + 50 )
       break; 
       }
       */
    }
    buffer[bix] = 0; // place string end marker 'null.'
  }
  // remaining characters incoming Serial RX will be lost
  // if signaling takes a long time. 
  if ( buffer[0] != 0 ) {
    for (bix = 0; buffer[bix] != 0; bix++) {
      char a = toupper(buffer[bix]);
      Serial.print(a);
      if ( a == ' ' ) {
        pause(7);
      } 
      else {
        char * p = code[a - '!'];
        char c;
        int i = 0; 
        do {
          c = p[i];
          switch(c) {
          case '1':
            signal(1);
            break;
          case '3':
            signal(3);
            break;
          default:
            pause(3);
            break;
          }
          pause(1);
          if ( a == 0x0d )
            Serial.println();
          i++;
        } 
        while (c != 0);
      }
    }
    buffer[0] = 0;
  } 
}









