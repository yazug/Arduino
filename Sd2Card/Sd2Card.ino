#include <SPI.h>
#include <Sd2Card.h>

Sd2Card sd;

void setup() {
  
  SPI.begin();
  sd.init(SPI_CLOCK_DIV8, 4);
  
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly: 
  
}
