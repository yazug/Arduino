#include <Wire.h>
#include "MCP23009.h"
#include <LiquidCrystal.h>
#include "xLCD.h"

const int PIN_LED = 13;
const int PIN_PIEZZO = 7;
long prev_pulse;
long mean_avr;

xLCD lcd(0x00, 1,0xff,2,3,4,5,6, 7);
//HC595LCD lcd(9, 7,6,5, 3,2,1,0);

// ISR variable and routine
volatile boolean got_pulse;

void int0srv() {
  got_pulse = true;
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_PIEZZO, OUTPUT);
  digitalWrite(PIN_LED, HIGH);

//  SPI.begin();
//  rtc.init();
  Wire.begin();
  lcd.begin();
  lcd.backlightOn();
  lcd.home();
  lcd.print("Starting...");
//  play();
  
  got_pulse = false;
  attachInterrupt(0, int0srv, FALLING);
  prev_pulse = millis();
  mean_avr = 0;
  lcd.clear();
}

void loop() {  
  if ( got_pulse ) {
    mean_avr = (mean_avr + (millis() - prev_pulse))/2;
    prev_pulse = millis();
    digitalWrite(PIN_LED, HIGH);
    tone(PIN_PIEZZO, 3150, 15);
    got_pulse = false;
    //
    lcd.home();
    /*
    lcd.print(clockval, HEX);
    lcd.print(" ");
    */
    lcd.print(60000L/mean_avr);    
    lcd.print("  ");
  }
  
  if ( digitalRead(PIN_LED) and (millis() > prev_pulse + 25) ) {
    digitalWrite(PIN_LED, LOW);
  }
}

const int
  NOTE_B0 = 31,
  NOTE_C1 = 33,
  NOTE_CS1 = 35,
  NOTE_D1 = 37,
  /*
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
*/
  NOTE_C6 = 1047,
  NOTE_CS6 = 1109,
  NOTE_D6 = 1175,
  NOTE_DS6 = 1245,
  NOTE_E6 = 1319,
  NOTE_F6 = 1397,
  NOTE_FS6 = 1480,
  NOTE_G6 = 1568,
  NOTE_GS6 = 1661,
  NOTE_A6 = 1760,
  NOTE_AS6 = 1865,
  NOTE_B6 = 1976,
  NOTE_C7 = 2093,
  NOTE_CS7 = 2217,
  NOTE_D7 = 2349,
  NOTE_DS7 = 2489,
  NOTE_E7 = 2637;
  /*
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
*/
const int toneFreq[] = {
  0, 
  NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6,
  NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6, 
  NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7
};

byte melody[] = {
  5, 16, 
  3, 16, 
  1, 8, 
  5, 8, 
  5, 8, 
  8, 8, 
  8, 8, 
  17, 8, 
  17, 16, 
  15, 16,
  13, 16,
  10, 16,
  8, 8,
  5, 8,
  13, 8,
  0, 16,
  5, 16,
  3, 4,
  0,  0
};

void play() {
  int i;
  const int baseT = 960 * 3;
  for (i = 0; melody[i<<1] or melody[(i<<1)+1]; i ++) {
    if ( melody[i<<1] ) {
      tone(PIN_PIEZZO, toneFreq[melody[i<<1]], 32);
    } else {
      delay(32);
    }
    delay(baseT/melody[(i<<1)+1] - 32);
  }
  noTone(PIN_PIEZZO);
}

