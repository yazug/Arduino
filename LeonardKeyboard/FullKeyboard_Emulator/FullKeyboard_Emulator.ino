/* keyPadHiduino Advanced Example Code
 by: Jim Lindblom
 date: January 5, 2012
 license: MIT license - feel free to use this code in any way
 you see fit. If you go on to use it in another project, please
 keep this license on there.
 */

#define KEY_LEFT_CTRL	0x80
#define KEY_LEFT_SHIFT	0x81
#define KEY_LEFT_ALT	0x82
#define KEY_LEFT_GUI	0x83
#define KEY_RIGHT_CTRL	0x84
#define KEY_RIGHT_SHIFT	0x85
#define KEY_RIGHT_ALT	0x86
#define KEY_RIGHT_GUI	0x87

#define KEY_UP_ARROW	0xDA
#define KEY_DOWN_ARROW	0xD9
#define KEY_LEFT_ARROW	0xD8
#define KEY_RIGHT_ARROW	0xD7
#define KEY_BACKSPACE	0xB2
#define KEY_TAB		0xB3
#define KEY_RETURN	0xB0
#define KEY_ESC		0xB1
#define KEY_INSERT	0xD1
#define KEY_DELETE	0xD4
#define KEY_PAGE_UP	0xD3
#define KEY_PAGE_DOWN	0xD6
#define KEY_HOME	0xD2
#define KEY_END		0xD5
#define KEY_CAPS_LOCK	0xC1
#define KEY_F1		0xC2
#define KEY_F2		0xC3
#define KEY_F3		0xC4
#define KEY_F4		0xC5
#define KEY_F5		0xC6
#define KEY_F6		0xC7
#define KEY_F7		0xC8
#define KEY_F8		0xC9
#define KEY_F9		0xCA
#define KEY_F10		0xCB
#define KEY_F11		0xCC
#define KEY_F12		0xCD

byte keycode, modifiers;

void sendKey(byte key, byte modifiers = 0);

void setup(){
  modifiers = 0;
  Keyboard.begin();
  Serial1.begin(19200);
  while (! Serial) ;
  Serial.begin(19200);
}

void loop() {
  byte code = 0;
  byte modif = 0;
  while ( Serial1.available() > 0 ) {
    code = Serial1.read();
    switch(code) {
    case 0x1c:
      code = KEY_LEFT_ARROW;
      break;
    case 0x1d:
      code = KEY_RIGHT_ARROW;
      break;
    case 0x1e:
      code = KEY_UP_ARROW;
      break;
    case 0x1f:
      code = KEY_DOWN_ARROW;
      break;
    case 0x1b:
      code = KEY_ESC;
      break;
    case 0x03:
      code = 0;
      modif = KEY_LEFT_CTRL;
      break;
    case 0x01:
      code = 0;
      modif = KEY_LEFT_ALT;
      break;
    case 0x13:
      code = 0;
      modif = KEY_RIGHT_SHIFT;
      break;
    }
    if ( code == 0 ) {
      modifiers |= modif;
    } 
    else {
      // send a CTRL+ALT+DEL to the computer via Keyboard HID
      sendKey(code, modifiers);
/*
      Serial.print(code>>4, HEX);
      Serial.print(code&0x0f, HEX);
      Serial.print('+');
      Serial.print(modifiers>>4, HEX);
      Serial.print(modifiers&0x0f, HEX);
      Serial.print(' ');
      */
      modifiers = 0;
    }
  }

}

void sendKey(byte key, byte modifiers)
{
  KeyReport report = {
    0              };  // Create an empty KeyReport

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

/*

*/






