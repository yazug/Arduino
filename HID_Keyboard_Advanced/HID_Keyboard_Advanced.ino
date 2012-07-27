/* keyPadHiduino Advanced Example Code
   by: Jim Lindblom
   date: January 5, 2012
   license: MIT license - feel free to use this code in any way
   you see fit. If you go on to use it in another project, please
   keep this license on there.
   
   This is an example of how to send keyboard modifiers and non-ascii
   characters using the Arduino HID keyboard class. You need to modify
   the class, in USBAPI.h (in the hardware\arduino\cores\arduino
   directory of your Arduino install), so that the sendReport() function
   of the Keyboard class is classified as a Public method.
   
   If you pull pin 9 low, the ProMicro will send a CTRL+ALT+DEL
   command to your computer. Windows users be ready for either the
   task manager or Windows Security screen. I'm not sure if it'll 
   do anything on a Mac or Linux box.
*/
#include "HID_Keyboard_Advanced.h";

void sendKey(byte key, byte modifiers = 0);
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
    sendKey(KEY_DELETE, 0x00_MODIFIER_LEFT_CTRL | KEY_MODIFIER_LEFT_ALT*/);  // send a CTRL+ALT+DEL to the computer via Keyboard HID
    delay(1000);  // Delay so as not to spam the computer
  }
}

void sendKey(byte key, byte modifiers)
{
  KeyReport report = {0};  // Create an empty KeyReport
  
  /* First send a report with the keys and modifiers pressed */
  report.keys[0] = key;  // set the KeyReport to key
  report.modifiers = modifiers;  // set the KeyReport's modifiers
  report.reserved = 1;
  Keyboard.sendReport(&report);  // send the KeyReport
  
  /* Now we've got to send a report with nothing pressed */
  for (int i=0; i<6; i++)
    report.keys[i] = 0;  // clear out the keys
  report.modifiers = 0x00;  // clear out the modifires
  report.reserved = 0;
  Keyboard.sendReport(&report);  // send the empty key report
}
