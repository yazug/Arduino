/*
*  Read the Barometric Pressure and Temperature values from SCP1000 sensor
*
*/

#include <SCP1000.h>

// Specify slave select pin for SCP1000 device
#define SelectPin 10

SCP1000 scp1000(SelectPin);

void setup()
{
   Serial.begin(9600); // Open serial connection to report values to host
   Serial.println("Starting up");
  
   scp1000.init();
}

void loop()
{

  scp1000.readSensor();
  

  Serial.print("Temprature (C/F): ");
  Serial.print(scp1000.TempC);
  Serial.print(" / ");
  Serial.println( (1.8*scp1000.TempC + 32) );
  Serial.print("Pressure (hPa): ");
  Serial.println(scp1000.BaroP);
  Serial.println("---------------------------------");

  delay(6000);
}
  

