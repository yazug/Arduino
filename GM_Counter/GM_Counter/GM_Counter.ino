#include <SPI.h>
#include <LiquidCrystal.h>
#include "DS3234.h"
#include "IOExpander.h"
#include "HC595iox.h"
#include "HC595LCD.h"
t
inline byte bcdToDec(byte b) { 
  return (b>>4)*10 + (b&0x0f); 
}

inline byte decToBcd(byte d) { 
  return ((d/10)<<4) + (d%10); 
}

//xLCD lcd(0x00, 1,0xff,2,3,4,5,6, 7);
HC595LCD lcd(9, 7,6,5, 3,2,1,0);


const byte rtc_cs = 10;
DS3234 rtc(rtc_cs);
struct Clock {
  DS3234 & rtc;
  long time;
  long calender;
  long lastupdateMillis;

  enum {
    MODE_BEEP = 0,
    MODE_BKLIGHT = 1,
    MODE_SEC = 3,
    MODE_CLOCKSET = 4
  };
  byte mode;
  byte clock_set;

  Clock(DS3234 & myRTC) : 
  rtc(myRTC) {
    init();
  }

  void init() {
    rtc.getTime(time);
    lastupdateMillis = millis();
    rtc.getCalender(calender);
    clock_set = 0;
  }

  boolean update() {
    long newTime;
    if ( millis() >= lastupdateMillis + 1000 ) {
      rtc.getTime(newTime);
      if ( newTime != time ) {
        time = newTime;
        lastupdateMillis = millis();
        return true;
      }
    }
    return false;
  }

  int seconds() { 
    return rtc.seconds(time); 
  }
  int minutes() { 
    return rtc.minutes(time); 
  }
  int hours() { 
    return rtc.hours(time); 
  }
  int day() { 
    return rtc.day(calender); 
  }
  int date() { 
    return rtc.date(calender); 
  }
  int month() { 
    return rtc.month(calender); 
  }
  int year() { 
    return rtc.year(calender); 
  }

  void setTime() { 
    rtc.setTime(time); 
  }
} 
myClock(rtc);

struct {
  unsigned int persec[60];
  long permin[60];
  long perhour[24];
  int prevsec;
  
  void init() {
    for(int i = 0; i < 60; i++) {
      persec[i] = 0;
      permin[i] = 0;
    }
    for(int i = 0; i < 24; i++) {
      perhour[i] = 0;
    }
  }
  
  int last60seconds(int sec) {
    int r = 0;
    for(int i = 0; i < 60; i++) {
      r += persec[(sec+i)%60];
    }
    return r/60;
  }
  
  long last60minutes(int m) {
    long r = 0;
    for(int i = 0; i < 60; i++) {
      r += permin[(m+i) % 60];
    }
    return r/60;
  }
} 
counter;

byte trend0[8] ={
  0,0,0,0,0,0,0,0}
,
trend1[8] = {
  0,0,0,0,0,0,0,0};

#define iomode(port, bits, mode)  ((mode) ? (port) |= ((byte)bits) : (port) &= ~((byte)bits) )
#define pin(n) ((n) >= 14 ? (1<<((n)-14)) : ((n) >= 8 ? (1<<((n)-8)) : (1<<(n)) ) )

const int led_red = 3; //PD3;
const int lcdlight = 5; //PD5;
const int led_blue = 6; //PD6;
const int led_green = 7; //PD7;
const int buzz = 8; //PB0;
const int button1 = 14; //PC0;
const int button2 = 15; //PC1;
const int button3 = 16; //PC2;

long prevPulseMillis;
int brightness;
byte button_state;
long button_changed;

// ISR variable and routine
volatile boolean gotPulse;

void int0srv() {
  gotPulse = true;
}

byte button_read() {
  return PINC & 0b111;
}

void setup() {
  Serial.begin(9600);

  Serial.println("Setup I/O");

  iomode(DDRC, pin(button1) | pin(button2) | pin(button3), INPUT);
  iomode(PORTC, pin(button1) | pin(button2) | pin(button3), HIGH);
  iomode(DDRD, pin(led_green) | pin(led_red) | pin(led_blue) | pin(lcdlight), OUTPUT);
  iomode(PORTD,  pin(led_green) | pin(led_red) | pin(led_blue) | pin(lcdlight), LOW);
  iomode(DDRB, pin(buzz), OUTPUT);

  Serial.println("init peripherals");

  SPI.begin();
  rtc.begin();
  lcd.begin();
  //  lcd.backlightOn();

  myClock.init();

  //Serial.begin(19200);
  delay(500);
  //pinMode(2, INPUT);
  //digitalWrite(2,HIGH);
  attachInterrupt(0,int0srv,FALLING);

  counter.init(); // clears seconds, minutes, hour counters
  Serial.println("counter cleared.");

  Serial.println("Start.");
  gotPulse = false;
  brightness = 120;
  prevPulseMillis = 0;

  display();
  //  play();

  analogWrite(lcdlight, brightness);
  myClock.mode = (1 <<Clock::MODE_BKLIGHT) | (1 <<Clock::MODE_BEEP);

  button_state = button_read();
  button_changed = millis();
}


void loop() {
  if ( gotPulse ) {
    brightness = 254;
    analogWrite(led_red, brightness);
    if ( bitRead(myClock.mode, Clock::MODE_BEEP) )
      tone(buzz, 3136,15);
    prevPulseMillis = millis();
    gotPulse = false;
    Serial.println("!");
    counter.persec[myClock.seconds()]++;
  } 
  else {
    if ( brightness > 0 && (millis() > prevPulseMillis + 2) ) {
      brightness--;
      analogWrite(led_red, brightness);
      prevPulseMillis += 2;
    }
  }

  byte btmp = button_read();
  long push_duration = 0;
  if ( btmp != button_state) {
    // button has been changed 
    if (btmp == 0b111) {
      push_duration = millis() - button_changed;
    }
    button_changed = millis();
    handle_button_event(btmp, push_duration);
    // update button state finally.
    button_state = btmp;
  }

  if ( millis() & 0x0f ) {
    if ( myClock.update() ) 
      display();
  }

}

void handle_button_event(byte btmp, long push_duration) {
  long tval;
  if ( push_duration <= 50 )
    return;
  if ( bitRead(myClock.mode, Clock::MODE_CLOCKSET) 
    or push_duration > 3000 ) {
    switch(button_state) {
    case 0b011:
      if ( bitRead(myClock.mode, Clock::MODE_CLOCKSET) ) {
        myClock.clock_set++;
        myClock.clock_set %= 7;
        if ( myClock.clock_set == 0 ) 
          bitClear(myClock.mode, Clock::MODE_CLOCKSET);
      } 
      else {
        myClock.clock_set = 1;
        bitSet(myClock.mode,Clock::MODE_CLOCKSET);
      }
      break;
    case 0b110:
    case 0b101:
      myClock.update();
      switch( myClock.clock_set ) {
      case 1:
        myClock.time &= 0xffffff00;
        break;
      case 2:
        if ( button_state == 0b110 ) {
          tval = (myClock.minutes()+1) % 60;
        } 
        else {
          tval = (myClock.minutes()+59) % 60;
        }
        tval = (tval / 10)* 0x10 + (tval % 10);
        myClock.time &= ~ (0xffL << 8); 
        myClock.time |= (tval<<8);
        break;
      case 3:
        if ( button_state == 0b110 ) {
          tval = (myClock.hours()+1) % 24;
        } 
        else {
          tval = (myClock.hours()+23) % 24;
        }
        tval = (tval / 10)* 0x10 + (tval % 10);
        myClock.time &= ~ (0xffL<<16); 
        myClock.time |= (tval<<16);
        break;
      case 4:
        if ( button_state == 0b110 ) {
          tval = myClock.date() % 31 + 1;
        } 
        else {
          tval = (myClock.date()+29) % 31 + 1;
        }
        tval = (tval / 10)* 0x10 + (tval % 10);
        myClock.calender &= ~ (0xffL<<8); 
        myClock.calender |= (tval<<8);
        break;
      case 5:
        if ( button_state == 0b110 ) {
          tval = myClock.month() % 12 + 1;
        } 
        else {
          tval = (myClock.month()+10) % 12 + 1;
        }
        tval = (tval / 10)* 0x10 + (tval % 10);
        myClock.calender &= ~ (0xffL<<16); 
        myClock.calender |= (tval<<16);
        break;
      case 6:
        if ( button_state == 0b110 ) {
          tval = (myClock.year()+1) % 100;
        } 
        else {
          tval = (myClock.year()+99) % 100;
        }
        tval = (tval / 10)* 0x10 + (tval % 10);
        myClock.calender &= ~ (0xffL<<24); 
        myClock.calender |= (tval<<24);
        break;
      }
      myClock.setTime();
      break;
    }
  } 
  else {
    switch(button_state) {
    case 0b110:
      if ( bitRead(myClock.mode,Clock::MODE_BEEP) ) {
        bitClear(myClock.mode,Clock::MODE_BEEP);
      } 
      else {
        bitSet(myClock.mode, Clock::MODE_BEEP);
      }
      break;
    case 0b101:
      if ( bitRead(myClock.mode,Clock::MODE_BKLIGHT) ) {
        bitClear(myClock.mode,Clock::MODE_BKLIGHT);
        brightness = 0;
      } 
      else {
        bitSet(myClock.mode,Clock::MODE_BKLIGHT);
        brightness = 120;
      }
      analogWrite(lcdlight, brightness);
      break;
    case 0b011:
      if ( bitRead(myClock.mode,Clock::MODE_SEC) ) {
        bitClear(myClock.mode,Clock::MODE_SEC);
      } 
      else {
        bitSet(myClock.mode,Clock::MODE_SEC);
      }
      break;
    }
  }
}


void display() {
  boolean minutes_update = false;
  char buf[8];
  long sum = 0;
  char c;

  lcd.home();
  if ( bitRead(myClock.mode, Clock::MODE_CLOCKSET) ) {
    if ( myClock.mode <= 3 ) {
      sprintf(buf,"        %02d:%02d:%02d", myClock.hours(), myClock.minutes(), myClock.seconds());
    } 
    else {
      sprintf(buf,"      20%02d/%02d/%02d  ", myClock.year(), myClock.month(),myClock.date() );
    }
    lcd.print(buf);
  } 
  else {
    if ( bitRead(myClock.mode, Clock::MODE_SEC) ) {
      sprintf(buf,"%02d/%02d  ", myClock.month(), myClock.date() );
      lcd.print(buf);
      sprintf(buf," %02d:%02d:%02d", myClock.hours(), myClock.minutes(), myClock.seconds());
      lcd.print(buf);
    } 
    else {
      lcd.print(DS3234::copyNameOfDay(buf, myClock.day()));
      lcd.print(' ');
      sprintf(buf,"%02d", myClock.date() );
      lcd.print(buf);
      lcd.print(' ');
      lcd.print(DS3234::copyNameOfMonth(buf,myClock.month()));
      lcd.print(' ');
      if ( !(myClock.time & 1) )
        c = ':';
      else
        c = ' ';
      sprintf(buf,"%02d%c%02d", myClock.hours(), c, myClock.minutes());
      lcd.print(buf);
    }
  }
  //  if ( (rtc.seconds(clock) % cps_size % 5) == 0 ) {
  lcd.setCursor(0, 1);
  lcd.print(counter.last60seconds(myClock.seconds()));
  lcd.print(" cpm ");
  /*
    lcd.print(cph[DS1307::hours(clock)]);
   lcd.print(" cph");
   */
  //  }
  if ( minutes_update ) {
    int t;
    for(int i = 0; i < 8; i++) {
      trend1[i] = (trend1[i]<<1)&0b11111;
    }
    for(int i = 0; i < 8; i++) {
      trend1[i] |= (trend0[i]&0b10000)>>4;
    }
    for(int i = 0; i < 8; i++) {
      trend0[i] = (trend0[i]<<1)&0b11111;
    }
    for(int i = 0; i < 8; i++) {
      if ( dlog(counter.last60minutes(myClock.minutes()), 2.72) >= i )
        trend0[7-i] |= 0b00001;
    }
    //
    lcd.createChar(0,trend1);
    lcd.createChar(1,trend0);
    lcd.setCursor(14,1);
    lcd.print(0,BYTE);
    lcd.print(1,BYTE);
    //lcd.home();
    //
  }

}

int dlog(long x, float b) {
  int n;
  if ( b > 1 ) {
    for (n = 0; n < sizeof(long)*8; n++) {
      if ( x < b ) 
        return n;
      x = long(x/b);
    }
  }
  return 0;
}





