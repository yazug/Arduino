#include <SPI.h>
#include <MCP320X.h>

MCP320X adc(10);

void setup() {
  Serial.begin(19200);
  Serial.println("Hi.");
  SPI.begin();
  adc.begin();
  adc.setupMode();
}

void loop() {
  word w[100];
  long micro[100];
  int i;
  for(i = 0; i < 100; i++) {
    adc.select();
    w[i] = adc.read12(0);
    micro[i] = micros();
    adc.deselect();
    delayMicroseconds(10);
  }

  for(i = 0; i < 100; i++) {
    Serial.print(micro[i]);
    Serial.print("\t");
    Serial.print(w[i]);
    Serial.println();
  }
  delay(2000);
}


