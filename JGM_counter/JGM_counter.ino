// include the library code:
//
#include <SPI.h>
#include "IOExpander.h"
#include <LiquidCrystal.h>
#include "HC595iox.h"
#include "HC595LCD.h"
#include "DS3234.h"
//
#include "PushButton.h"

byte decToBCD(byte d) { 
  return ((d/10)<<4) + (d%10); 
}

const int PIN_BUZZ = 8; // PB0
const int PIN_595_CS = 9; //PB1
const int PIN_RTC_CS = 10; //PB2
const int PIN_LCDLIGHT = 5; // PD5
const int PIN_LEDRED = 3; //PD3
const int PIN_LEDGREEN = 7; //PD7
const int PIN_LEDBLUE = 6;
const int PIN_SW1 = 14; //PC0;
const int PIN_SW2 = 15; //PC1;
const int PIN_SW3 = 16; //PC2;
const int pin_pulseIn = 2; // INT0

HC595LCD lcd(PIN_595_CS, 7,6,5, 3,2,1,0);
DS3234 rtc(PIN_RTC_CS);

boolean display_requestRefresh = true;
boolean display_lightOn = true;
boolean display_seconds = false;
long clockVal;
long calendarVal;

volatile unsigned int counter_pulses;
volatile boolean pulsed;
void INT0_srv() {
  pulsed = true;
  counter_pulses++;
}

long cpmavr = 0;
int lastSeconds[60];
//long last1minPulses;
long lastMinutes[60];
long lastDisplayRefreshed;
long lastPulsed;
long lastPulseLit;
byte led_int;

enum {
  SW1 = 0,
  SW2,
  SW3 };
const int numOfButtons = 3;
PushButton buttons[] = { 
  PIN_SW1, PIN_SW2, PIN_SW3 };

boolean beep_soundOn = true;
enum Mode {  
  NONE = 0,
  SECOND,
  MINUTE,
  HOUR,
  DATE,
  MONTH,
  YEAR
} 
mode_adjust = NONE;
Mode & operator ++(Mode & m) {
  return m = static_cast<Mode>( static_cast<byte>(m) % YEAR + 1 );
}

void setup() {
  pinMode(PIN_BUZZ, OUTPUT);
  pinMode(PIN_LCDLIGHT, OUTPUT);
  pinMode(PIN_LEDRED, OUTPUT);
  pinMode(PIN_LEDGREEN, OUTPUT);
  pinMode(PIN_LEDBLUE, OUTPUT);
  //
  for(int i = 0; i < numOfButtons; i++) {
    buttons[i].init();
  }
  //
  SPI.begin();
  lcd.begin(16, 2);
  rtc.begin();
  lcd.print("hello, world!");
  //
  counter_pulses = 0;
  attachInterrupt(INT0, INT0_srv, FALLING );
  //
  // set status
  rtc.getTime(clockVal);
  rtc.getCalendar(calendarVal);
  lastDisplayRefreshed = millis()/100;
  // initialize pulse count registers
  for(int i = 0; i < 60; i++) {
    lastSeconds[i] = 0;
    lastMinutes[i] = 0;
  }
  //
  pulsed = false;
  lcd.clear();
  //
  //Serial.begin(19200);
}

void loop() {
  long temp_clock;

  if ( pulsed ) {
    lastPulsed = millis();
    if ( beep_soundOn )
      tone(PIN_BUZZ, 2200);
    led_int = 255;
    lastPulseLit = millis();
    analogWrite(PIN_LEDRED, led_int);
    pulsed = false;
  }
  else {
    if ( millis() > 15 + lastPulsed) {
      noTone(PIN_BUZZ);
    }
  }
  if ( led_int > 0 and millis() > lastPulseLit + 31) {
    lastPulseLit = millis();
    led_int = led_int * 7 / 8;
    analogWrite(PIN_LEDRED, led_int);
    //    analogWrite(PIN_LEDBLUE, led_int*0.5);
  }

  // refresh the display contents.
  if ( millis()>>5 != lastDisplayRefreshed ) {
    rtc.getTime(temp_clock);
    if ( clockVal != temp_clock )
      display_requestRefresh = true;
    if ( display_requestRefresh ) {
      lastSeconds[ clockVal & 0x3f ] = counter_pulses;
      int m = clockVal >> 8 & 0x3f;
      lastMinutes[ m ] = 0;
      for(int i = 0; i < 60; i++) {
        lastMinutes[ m ] += lastSeconds[i];
      }
      counter_pulses = 0;
      clockVal = temp_clock;
      lastSeconds[clockVal & 0x3f ] = counter_pulses;
      //
      if ( clockVal & 0xffff == 0 )
        rtc.getCalendar(calendarVal);
      //
      display(clockVal, calendarVal, lastMinutes[m]);
      //
      display_requestRefresh = false;
      lastDisplayRefreshed = millis()>>5;
    }
  }

  // scan buttons
  for (int i = 0; i < numOfButtons; i++) 
    buttons[i].check();
  //
  if ( buttons[SW1].changed() && buttons[SW1].isLow() ) {
    // SW1 short push down
    if ( mode_adjust == NONE ) {
      display_lightOn = !display_lightOn;
    } 
    else {
      clock_adjust();
    }
  } 
  else if ( buttons[SW1].isLow() && buttons[SW1].continuing() > 1950 
    && mode_adjust) {
    clock_adjust();
    delay(125);
  }
  if ( buttons[SW2].changed() && buttons[SW2].isLow() ) {
    // SW2 short push down
    if ( mode_adjust == NONE ) {
      beep_soundOn = !beep_soundOn;
    } 
    else {
      ++mode_adjust;
      lcd.setCursor(0,1);
      char msg[][6] = {
        "none", "sec", "min", "hour", "day", "month", "year"
      };
      lcd.print(msg[static_cast<int>(mode_adjust)]);
    }
  }
  if ( buttons[SW3].changed() ) {
    if ( buttons[SW3].isLow() ) {
      if ( mode_adjust == NONE ) {
        display_seconds = !display_seconds;
      }
    } 
    else if ( buttons[SW3].isHigh() && buttons[SW3].continued() > 1200 ) {
      if ( mode_adjust == NONE ) {
        mode_adjust = SECOND;
      } 
      else {
        mode_adjust = NONE;
      }
      digitalWrite(PIN_LEDBLUE, mode_adjust != NONE);
    }
  }

}

void clock_adjust() {

  byte s = DS3234::seconds(clockVal);
  byte m = DS3234::minutes(clockVal);
  byte h = DS3234::hours(clockVal);
  byte d = DS3234::day(calendarVal);
  byte mo = DS3234::month(calendarVal);
  byte y = DS3234::year(calendarVal);
  /*
  Serial.print(h);
   Serial.print(',');
   Serial.print(m);
   Serial.print(',');
   Serial.print(s);
   Serial.println();
   */
  switch ( mode_adjust ) {
  case SECOND:
    s = 0;
    break;
  case MINUTE:
    m++;
    m %= 60;
    break;
  case HOUR:
    h++;
    h %= 24;
    break;
  case DATE:
    switch(mo) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
      d = (d % 31) + 1;
      break;
    case 2:
      d = (d % 28) + 1;
      break;
    default:
      d = (d % 30) + 1;
      break;
    }
    break;
  case MONTH:
    mo = (mo % 12) + 1;
    break;
  case YEAR:
    y++;
    y %= 100;
    break;
  default:
    break;
  }
  switch(mode_adjust) {
  case SECOND:
  case MINUTE:
  case HOUR:
    clockVal = long(decToBCD(h))<<16 | decToBCD(m)<<8 | decToBCD(s);
    rtc.setTime(clockVal);
    break;
  case DATE:
  case MONTH:
  case YEAR:
    calendarVal = long(decToBCD(y))<<24 | decToBCD(mo)<<16 | decToBCD(d)<<8;
    rtc.setCalendar(calendarVal);
    break;
  }
  display(clockVal, calendarVal, lastMinutes[m]);
}

unsigned int dlog(unsigned long x) {
  unsigned int result = 0;
  do {
    x /= 10;
    result++;
  } 
  while ( x > 0 );
  return result;
}

void display(long time, long date, long cpm) {
  char tmpstr[8];

  lcd.setCursor(0, 0);
  lcd.print(time>>20&0x0f, HEX);
  lcd.print(time>>16&0x0f, HEX);
  if ( display_seconds ) {
    lcd.print(':');
  } 
  else {
    if ( time & 1 )
      lcd.print(' ');
    else
      lcd.print(':');
  }
  lcd.print(time>>12&0x0f, HEX);
  lcd.print(time>>8&0x0f, HEX);
  if ( display_seconds ) {
    lcd.print(':');
    lcd.print(time>>4&0x0f, HEX);
    lcd.print(time&0x0f, HEX);
    lcd.print("  ");
  } 
  else {
    lcd.print(" ");
    lcd.print(DS3234::copyNameOfDay(tmpstr, date&0x0f));
  }
  lcd.print(' ');
  if ( display_seconds ) {
    lcd.print(date>>20&0xf, HEX);
    lcd.print(date>>16&0xf, HEX);
    lcd.print('/');
    lcd.print(date>>12&0xf, HEX);
    lcd.print(date>>8&0xf, HEX);
  } 
  else {
    lcd.print(date>>12&0xf, HEX);
    lcd.print(date>>8&0xf, HEX);
    lcd.print(' ');
    lcd.print(DS3234::copyNameOfMonth(tmpstr, date>>16&0x0f) );
  }
  if ( mode_adjust == NONE ) {
    cpmavr += cpm;
    cpmavr >>= 1;
    lcd.setCursor(0,1);
    for(int i = 0; i < max(0,10 - dlog(cpmavr)); i++)
      lcd.print(' ');
    lcd.print(cpmavr);
    lcd.print(" [cpm]");
  }
  digitalWrite(PIN_LCDLIGHT, display_lightOn);
  //digitalWrite(PIN_LEDGREEN, beep_soundOn);
}
































