// example use of LCDI2C library with the web4robot.com i2C lcd display
// On most Arduino boards, SDA (data line) is on analog input pin 4, and
// SCL (clock line) is on analog input pin 5. 
  
#include <Wire.h>
#include "CharacterLCD.h"
#include "ST7032i.h"
#include "M41T62.h"

LCD_ST7032i lcd;             // Number of lines and i2c address of the display
M41T62 rtc;

int a2history[12];
unsigned int a2index = 0;

void setup() { 

  Serial.begin(38400);
  
  // disable i2c level translator on xbee board
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  
  Wire.begin();
  rtc.begin();
  lcd.begin();                          // Init the display, clears the display 
  lcd.print("Hello!");       // Classic Hello World!
  lcd.setContrast(28);
  for(int i = 0; i < 2; i++) {
    lcd.noDisplay();
    delay(500);
    lcd.display();
    delay(500);
  }
  lcd.clear();
  rtc.updateTime();
  lcd.print(rtc.time, HEX);
  /*
  byte bitmap[] = {1,2,3,4,5,6,7,8};
  lcd.createChar(0, bitmap);
  lcd.setCursor(14,0);
  lcd.write((uint8_t)0);
*/  
}

void loop() {
  int a[3], base[3];
  long sum[3];

  const int samples = 103;
  int i;
  sum[0] = 0; sum[1] = 0; sum[2] = 0;
  for(i = 0; i < samples; i++) {
    sum[0] += analogRead(0);
    sum[1] += analogRead(1);
    sum[2] += analogRead(2);    
    delay(11);
  }
  
  base[0] = sum[0] / samples;
  base[1] = sum[1] / samples;
  base[2] = sum[2] / samples;
  
  Serial.print(base[0]);
  Serial.print(", ");
  Serial.print(base[1]);
  Serial.print(", ");
  Serial.print(base[2]);
  Serial.println();

  sum[0] = 0; sum[1] = 0; sum[2] = 0;
  for(i = 0; i < samples; i++) {
    sum[0] += abs(analogRead(0) - base[0]);
    sum[1] += abs(analogRead(1) - base[1]);
    sum[2] += abs(analogRead(2) - base[2]);
    delay(11);
  }
  Serial.print(sum[0]/(samples));
  Serial.print(", ");
  Serial.print(sum[1]/(samples));
  Serial.print(", ");
  Serial.print(sum[2]/(samples));
  Serial.println();
  Serial.println();
  
  rtc.updateTime();
//  a2history[(a2index++) % 12] = abs(a2max - a2min)/count;
  lcd.setCursor(0,0);
  lcd.print(rtc.time, HEX);
  lcd.setCursor(0, 1);
  lcd.print(sum[1]/(samples)); lcd.print(" ");
  lcd.print(sum[2]/(samples)); lcd.print(" ");
  delay(1000);
}


int adc(const byte pin = 0) {
  byte low;
  word high;

  ADMUX = (DEFAULT << 6) | (pin & 0x07);

  // start the conversion
  ADCSRA |= (1<<ADSC);

  // ADSC is cleared when the conversion finishes
  while ( ADCSRA & (1<< ADSC));

  // we have to read ADCL first; doing so locks both ADCL
  // and ADCH until ADCH is read.  reading ADCL second would
  // cause the results of each conversion to be discarded,
  // as ADCL and ADCH would be locked when it completed.
  low  = ADCL;
  high = ADCH;

  return (high << 8) | low;
}

float getPower(byte pch, byte gndch) {
  long v0sum = 0;
  for (int i = 0; i < 64; i++) {
    v0sum += adc(gndch);
    delayMicroseconds(21);
  }
  int gndavr = v0sum / 64;
  v0sum = 0;
  for (int i = 0; i < 64; i++) {
    int d = adc(gndch) - gndavr;
    v0sum += d*d;
    delayMicroseconds(267);
  }
  v0sum = sqrt(v0sum/64/63);

  unsigned int diff;
  long sum = 0;
  for (int i = 0; i < 256; i++) {
    diff = adc(pch) - gndavr; 
    sum += diff*diff;
    delayMicroseconds(21);
  }

  return sqrt(sum/256);

}

