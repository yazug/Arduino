/*
 * Query a SHT10 temperature and humidity sensor
 *
 * A simple example that queries the sensor every 3 seconds
 * and communicates the result over a serial connection.
 * Error handling is omitted in this example.
 */

#include "Sensirion.h"

#define DATA_PIN  3
#define SCK_PIN   2

int temperature;
int humidity;
float dewpoint;

Sensirion tempSensor = Sensirion(DATA_PIN, SCK_PIN);

void setup() {
  Serial.begin(9600);

  Serial.print("Temperature: \t");
  Serial.print("Humidity: \t");
  //Serial.print(" %, Dewpoint: ");
  Serial.println();
}

void loop()
{
  tempSensor.measure(&temperature, &humidity);
  
  Serial.print((float)temperature/100);
  Serial.print(" C\t\t");
  Serial.print((float)humidity/100);
  Serial.print(" %\t\t");
  Serial.print(analogRead(2));
  Serial.println();
  delay(3000);  
}
