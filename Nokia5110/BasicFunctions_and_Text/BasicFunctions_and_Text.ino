#include "Adafruit_GFX.h"
#include "Adafruit_PCD8544.h"

// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 3 - LCD chip select (CS)
// pin 4 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 3, 4);

void setup(void) {
  Serial.begin(9600);

  display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(48);

  display.display(); // show splashscreen
  delay(2000);
  display.clear();   // clears the screen and buffer

    // text display tests
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.println("Hello, world!");
  display.setTextColor(WHITE, BLACK); // 'inverted' text
  display.println(3.141592);
  display.setTextSize(2);
  display.setTextColor(BLACK);
  display.print("0x"); 
  display.println(0xDEADBEEF, HEX);
  display.display();
  delay(2000);

  // invert the display
  /*
  display.invertDisplay(true);
   delay(1000); 
   display.invertDisplay(false);
   delay(1000); 
   */
}

void loop(void)
{
  display.clear();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.print(millis());
  display.setCursor(0,1*8);
  display.print("?");
  display.display();
  delay(100);
} 

void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    //if ((i > 0) && (i % 14 == 0))
    //display.println();
  }    
  display.display();
}


