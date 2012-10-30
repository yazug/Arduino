// example use of LCDI2C library with the web4robot.com i2C lcd display
// On most Arduino boards, SDA (data line) is on analog input pin 4, and
// SCL (clock line) is on analog input pin 5. 
  
#include <Wire.h>
#include "CharacterLCD.h"
#include "ST7032i.h"

LCD_ST7032i lcd;             // Number of lines and i2c address of the display

int a2history[12];
unsigned int a2index = 0;

void setup() { 

  Serial.begin(38400);
  
  // disable i2c level translator on xbee board
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  
  Wire.begin();
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
  
//  a2history[(a2index++) % 12] = abs(a2max - a2min)/count;
  lcd.setCursor(0, 0);
  lcd.print(sum[1]/(samples)); lcd.print("    ");
  lcd.setCursor(0,1);
  lcd.print(sum[2]/(samples)); lcd.print("    ");
  delay(1000);
}
