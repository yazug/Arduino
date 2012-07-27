/*
 * Query a SHT10 temperature and humidity sensor
 *
 * A simple example that queries the sensor every 3 seconds
 * and communicates the result over a serial connection.
 * Error handling is omitted in this example.
 */
#include <stdio.h>

#include "Sensirion.h"

#define DATA_PIN  5
#define SCK_PIN   2

float temperature;
float humidity;
float dewpoint;
char m[256];

Sensirion tempSensor = Sensirion(DATA_PIN, SCK_PIN);

void setup()
{
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);

  Serial.begin(9600);
}

void loop()
{
  tempSensor.measure(&temperature, &humidity, &dewpoint);
  
  sprintf(m,"%f %f", temperature, humidity);

  Serial.print("Temperature: ");
  Serial.print(temperature,2);
  Serial.print(" C, Humidity: ");
  Serial.print(humidity,2);
  Serial.print(" %, Dewpoint: ");
  Serial.print(dewpoint,2);
  Serial.println(" C");
  
  Serial.println(m);
  
  delay(3000);  
}

