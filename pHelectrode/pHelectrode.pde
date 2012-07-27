/*
 pHduino.pde - pHduino firmware for Arduino.
 Copyright (c) 2009 Carlos A. Neves
 http://code.google.com/p/phduino
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 Version 0.4_20090920
 Author: Carlos A. Neves <caneves at google>
 
 */
#include <avr/eeprom.h>

#include "WProgram.h"
#include "pHduino.h"

#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include "SerLCD.h"
SerLCD slcd(2, 2, 16);

// Creates the LCD instance
//LiquidCrystal lcd(PH_LCD_RS_PIN, PH_LCD_RW_PIN, PH_LCD_E_PIN, PH_LCD_D4_PIN, PH_LCD_D5_PIN, PH_LCD_D6_PIN, PH_LCD_D7_PIN);

// ordinary variables
char myStr[16];
int val_E;
int Vref;
int val_T;
float E;
float R;
float T;
float pH;
int i;
float f, f2;

unsigned long int my_timer0;
unsigned long int my_timer;

boolean pH_alarm_status = false;

// struct of parameters
struct myRec_T{
  boolean start_flag;
  unsigned int time_between_acquisitions_ms;
  boolean temperature_sensor_flag;
  float temperature_constant_celcius;
  boolean pH_alarm_flag;
  float pH_inf;
  float pH_sup;
} 
myRec;

void parameters_reset(void){

  myRec.start_flag = true;
  myRec.time_between_acquisitions_ms = PH_INITIAL_TIME_BETWEEN_ACQUISITIONS_MS;
  myRec.temperature_sensor_flag = true;
  myRec.temperature_constant_celcius = PH_TEMPERATURE_VALUE_INITIAL_CELCIUS;
  myRec.pH_alarm_flag = false;
  myRec.pH_inf = 0.0;
  myRec.pH_sup = 14.0;

  eeprom_write_block(&myRec, (void *)0, sizeof(myRec));
}

void setup(void){

  // Init serial port
//  Serial.begin(19200);
  slcd.begin(9600);
  slcd.brightness(15);

  // setup LED, button, and buzzer
  pinMode(PH_LED_PIN, OUTPUT);
//  pinMode(PH_BUTTON_CTRL_PIN, INPUT);
//  pinMode(PH_BUZZER_PIN, OUTPUT);

  // initialize LED and buzzer
  digitalWrite(PH_LED_PIN, LOW);
//  digitalWrite(PH_BUZZER_PIN, LOW);

  // begin LCD

  // Initial messagens.
//  digitalWrite(PH_BUZZER_PIN, HIGH);
  delay(200);
//  Serial.println("Waking up...");
//  digitalWrite(PH_BUZZER_PIN, LOW);

  digitalWrite(PH_LED_PIN, HIGH);
  //
  slcd.home();
//  lcd.print("pHduino");
//  lcd.setCursor(0,1);
  slcd.print("Firm ver. ");
  slcd.print(PH_PHDUINO_VERSION); 
  slcd.setCursor(0,1);
  slcd.print("Reseting...");
  delay(2000);
  digitalWrite(PH_LED_PIN, LOW);

  // read parameters from EEPROM
  eeprom_read_block(&myRec, (void *)0, sizeof(myRec));
  
  //Serial.println("Let's reset.");
  parameters_reset();

  slcd.clear();

  // Free memory check
  //Serial.println(availableMemory());
}

void process_data(void){

  digitalWrite(PH_LED_PIN, HIGH);

  // print temperature
  // Obs.: You can operate the pHduino without a temperature sensor.
  // In this case, you need setup a temperature value 
  // and comment the readADC and calc lines.

  myRec.temperature_sensor_flag = false;
  if (myRec.temperature_sensor_flag == true){
    val_T = readADC(PH_TEMP_ADC_PIN, PH_TIME_ADC_READINGS_MICROSECONS);
//    T = 100 * val_T * 5.0 / 1024;
    T = -115 + (float) val_T / 0.97;
  }
  else
  {
    T = myRec.temperature_constant_celcius; // celcius temperature to operates without a temperature sensor.
  }

  Vref = readADC(PH_REF_ADC_PIN, PH_TIME_ADC_READINGS_MICROSECONS);
  R = PH_GAIN_STAGE2_REF * Vref;

  slcd.setCursor(0,0);
  slcd.print(T,1);
  slcd.print((char) 0xdf);
  slcd.print("C ");
  
  // print ADC value and voltage of electrochemical potential
  val_E = readADC(PH_PH_ADC_PIN, PH_TIME_ADC_READINGS_MICROSECONS);
//  E = -1 * ((val_E * 5.0 / 1024) - R) / PH_GAIN_STAGE1_PH;
  E = -1 * ((val_E - PH_GAIN_STAGE2_REF *Vref) * 5.0 / 1024) / PH_GAIN_STAGE1_PH;

/*
  slcd.setCursor(0,0);
  slcd.print("adc ");
  slcd.print(val_E);
  slcd.print(" ");
  */
  //slcd.setCursor(0, 1);
  slcd.print((int)(E*1000)); // mili volts
  slcd.print("/");
  slcd.print(Vref); // mili volts

  // print pH
  // delta_pH = -(F/(2.303*R*T))*delta_E
  // <http://en.wikipedia.org/wiki/PH>
  // where,
  // gas constant R = 8.31451 J * K^-1 * mol^-1 <http://en.wikipedia.org/wiki/Gas_constant>
  // Kelvin temperature T = celcius temperature plus 273.15 K <http://en.wikipedia.org/wiki/Kelvin>
  // Faraday constant F = 96485 C <http://en.wikipedia.org/wiki/Faraday_constant>
  pH = (float)((-5038.8 * E / (273.15 + T)) + 7.0);
  pH = (float)(-Vref*2+val_E)/132.9*0.99+6.69;

//  floatToString(myStr, pH, 2);

  slcd.setCursor(0, 1);
  slcd.print("pH ");
  slcd.print(pH,2);
  slcd.print(" ");

  //Serial.print("pH: ");
  //Serial.print(pH, 2);
  //Serial.print(" ");

  // print alarm signal
  if (myRec.pH_alarm_flag == true){
    //Serial.print(1, DEC);
    //Serial.print(" ");
    if ((pH < myRec.pH_inf) || (pH > myRec.pH_sup)){
     // Serial.print(1, DEC);
      pH_alarm_status = true;
 //     lcd.setCursor(7, 0);
//      lcd.print("*");
    }
    else{
      //Serial.print(0, DEC);
//      pH_alarm_status = false;
//      lcd.setCursor(7, 0);
//      lcd.print(" ");
    }
  }
  else{
    //Serial.print(0, DEC);
    //Serial.print(" ");
    //Serial.print(0, DEC);
    pH_alarm_status = false;
  }

  //Serial.println();

  if (pH_alarm_status == false){
    digitalWrite(PH_LED_PIN, LOW);
  }
  else{
//    digitalWrite(PH_BUZZER_PIN, HIGH);
//    delay(100);
//    digitalWrite(PH_BUZZER_PIN, LOW);
  }
}

void loop(void){

  my_timer = millis();
  if ((my_timer-my_timer0) > myRec.time_between_acquisitions_ms){

    // reset the timer
    my_timer0 = my_timer;

    if (myRec.start_flag == true){
      process_data();  
    }
  }
  
//  process_cmd();  
}


