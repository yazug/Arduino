/* keyPadHiduino Advanced Example Code
   by: Jim Lindblom
   date: January 5, 2012
   license: MIT license - feel free to use this code in any way
   you see fit. If you go on to use it in another project, please
   keep this license on there.
*/

#define KEY_DELETE 0x4C
#define KEY_LEFT 0x50

int buttonPin = 9;  // Set a button to digital pin 9

void setup()
{
  pinMode(buttonPin, INPUT);  // Set the button as an input
  digitalWrite(buttonPin, HIGH);  // Pull the button high
}

void loop()
{
  if (digitalRead(buttonPin) == 0)  // if the button goes low
  {
    Keyboard.sendKey(KEY_LEFT, 0); //, KEY_MODIFIER_LEFT_CTRL | KEY_MODIFIER_LEFT_ALT);  // send a CTRL+ALT+DEL to the computer via Keyboard HID
    delay(100);  // Delay so as not to spam the computer
  }
}


