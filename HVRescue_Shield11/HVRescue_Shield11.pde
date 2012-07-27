/*
  Title:        HVRescue_Shield
  Description:  Arduino sketch for use with the mightyohm.com AVR HV Rescue Shield
  Author:       Jeff Keyzer
  Company:      MightyOhm Engineering
  Website:      http://mightyohm.com
  Contact:      http://mightyohm.com/blog/contact/  
  
  This sketch assumes that the Arduino is equipped with an AVR HV Rescue Shield.
  Schematics and other details are available at http://mightyohm.com/rescue_shield/

  The sketch uses the AVR High Voltage Programming Mode to set fuses on
  the Atmel ATmega48/88/168/328 and ATtiny2313 AVR microcontrollers.
  The HV programming routines are based on those described in the
  ATmega48/88/168 datasheet rev. 2545M-AVR-09/07, pg. 290-297 and the ATtiny2313
  datasheet rev. 2543I-AVR-04/06 pg. 165-176.  These routines may be compatible
  with other members of the AVR family, but this has not been tested.
  
  Changelog:
  11/02/09 v1.1
    - Removed endSerial call after reading back fuse bytes, was spewing garbage into
      serial monitor
    - Still occsionally get garbage when opening serial monitor, not sure what is causing this.
    
  03/01/09 v1.0
    - ATtiny2313 support, enable with ATtiny option
    - 12V Step up converter enable is non-inverting, unlike previous level shifter circuit
    - added interactive mode, asks for fuse values to burn, option to turn off
    - added EFUSE support and option to enable
    - button now has very simple debounce routine
    
  09/24/08
    - original release of sketch "HVFuse" to support first implementation on perfboard
    - Details: http://mightyohm.com/blog/2008/09/arduino-based-avr-high-voltage-programmer/
  
  Copyright 2008, 2009 Jeffrey S. Keyzer
 
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
*/

// User defined settings
#define  ATtiny       0     // Set this to 1 to flash the ATtiny2313 (default is ATmega48/88/168)
#define  INTERACTIVE  0     // Set this to 0 to disable interactive (serial) mode
#define  BURN_EFUSE   0     // Set this to 1 to enable burning extended fuse byte
#define  BAUD         9600  // Serial port rate at which to talk to PC

// If interactive mode is off, these fuse settings are used instead of user prompted values
#define  LFUSE        0x62  // default for ATmega168 = 0x62
#define  HFUSE        0xD9  // default for ATmega168 = 0xDF
#define  EFUSE        0xFF  // default for ATmega168 = 0xF9

// Pin Assignments (you shouldn't need to change these)
// Note: Analog inputs 0-5 can be addressed as digital outputs 14-19
#define  DATA    PORTD // PORTD = Arduino Digital pins 0-7
#define  DATAIN  PIND  // Corresponding inputs
#define  DATAD   DDRD  // Data direction register for DATA port
#define  VCC     12
#define  RDY     13     // RDY/!BSY signal from target
#define  OE      11
#define  WR      10
#define  BS1     16
#define  XA0     8
#define  XA1     18    
#define  RST     14    // 12V Step up converter enable (12V_EN)
#define  XTAL1   17
#define  BUTTON  15    // Run button
// Target-specific Pin Assignments 
#if (ATtiny == 1)
  #define  PAGEL   16   // same as BS1
  #define  BS2     18  // same as XA1
#else
  #define  PAGEL   19
  #define  BS2     9
#endif

// Internal definitions
#define  LFUSE_SEL  0
#define  HFUSE_SEL  1
#define  EFUSE_SEL  2

void setup()  // run once, when the sketch starts
{
  // Set up control lines for HV parallel programming
  DATA = 0x00;  // Clear digital pins 0-7
  DATAD = 0xFF; // set digital pins 0-7 as outputs
  pinMode(VCC, OUTPUT);
  pinMode(RDY, INPUT);
  pinMode(OE, OUTPUT);
  pinMode(WR, OUTPUT);
  pinMode(BS1, OUTPUT);
  pinMode(XA0, OUTPUT);
  pinMode(XA1, OUTPUT);
  pinMode(PAGEL, OUTPUT);
  pinMode(RST, OUTPUT);  // signal to level shifter for +12V !RESET
  pinMode(BS2, OUTPUT);
  pinMode(XTAL1, OUTPUT);
  
  pinMode(BUTTON, INPUT);
  digitalWrite(BUTTON, HIGH);  // turn on internal pullup resistor

  // Initialize output pins as needed
  digitalWrite(RST, LOW);  // Turn off 12V step-up converter (non-inverting, unlike original circuit
  digitalWrite(VCC, LOW); 
}

void loop()  // run over and over again
{
  byte hfuse, lfuse, efuse;  // desired fuse values from user
  byte read_hfuse, read_lfuse, read_efuse;  // fuses read from target for verify
  
#if (INTERACTIVE == 1)
  DATAD = 0x00;  // Set all DATA lines to input, including UART lines (so we can use them for serial comm)
  Serial.begin(BAUD);
  delay(10);
  //Serial.print("\n");
  Serial.println("Insert target AVR and press button.");
#endif

  // wait for button press, debounce
  while(1) {
    while (digitalRead(BUTTON) == HIGH);  // wait here until button is pressed
    delay(10);                            // simple debounce routine
    if (digitalRead(BUTTON) == LOW)       // if the button is still pressed, continue
      break;  // valid press was detected, continue on with rest of program
  }
  
  // Initialize pins to enter programming mode
  DATA = 0x00;
  DATAD = 0xFF;  // Set all DATA lines to output mode
  digitalWrite(PAGEL, LOW);
  digitalWrite(XA1, LOW);
  digitalWrite(XA0, LOW);
  digitalWrite(BS1, LOW);
  digitalWrite(BS2, LOW);
  digitalWrite(WR, LOW);  // ATtiny needs this to be low to enter programming mode, ATmega doesn't care
 
  // Enter programming mode
  digitalWrite(VCC, HIGH);  // Apply VCC to start programming process
  delay(1);
  digitalWrite(OE, HIGH);
  delay(1);
  digitalWrite(RST, HIGH);   // Apply 12V to !RESET thru level shifter
  delay(1);
  digitalWrite(WR, HIGH);   // Now that we're in programming mode we can disable !WR

  // Now we're in programming mode until RST is set LOW again
  
#if (INTERACTIVE == 1)
  // Ask the user what fuses should be burned to the target
  // For a guide to AVR fuse values, go to http://www.engbedded.com/cgi-bin/fc.cgi
  // Serial.println("Ensure target AVR is removed before entering fuse values."); 
  Serial.print("Enter desired LFUSE hex value (ie. 0x62): ");
  lfuse = fuse_ask();
  Serial.print("Enter desired HFUSE hex value (ie. 0xDF): ");
  hfuse = fuse_ask(); 
  #if (BURN_EFUSE == 1)
    Serial.print("Enter desired EFUSE hex value (ie. 0xF9): ");
    efuse = fuse_ask();
  #endif 
  endSerial();    // We're done with serial comms (for now) so disable UART
#else
  hfuse = HFUSE;
  lfuse = LFUSE;
  efuse = EFUSE;
#endif

  // Now burn desired fuses
  // First, program HFUSE
  fuse_burn(hfuse, HFUSE_SEL);

  // Now, program LFUSE
  fuse_burn(lfuse, LFUSE_SEL);

#if (BURN_EFUSE == 1)
  // Lastly, program EFUSE
  fuse_burn(efuse, EFUSE_SEL);
#endif

  //delay(1000);  // wait a while to allow button to be released

  // Read back fuse contents to verify burn worked
  read_lfuse = fuse_read(LFUSE_SEL);
  read_hfuse = fuse_read(HFUSE_SEL);
  #if (BURN_EFUSE == 1)
    read_efuse = fuse_read(EFUSE_SEL);
  #endif


  // Done verifying
  //digitalWrite(OE, HIGH);

  DATAD = 0x00;
  Serial.begin(BAUD);  // open serial port
  Serial.print("\n");  // flush out any garbage data on the link left over from programming
  Serial.print("Read LFUSE: ");
  Serial.print(read_lfuse, HEX);
  Serial.print("\n");
  Serial.print("Read HFUSE: ");
  Serial.print(read_hfuse, HEX);
  Serial.print("\n");
  #if (BURN_EFUSE == 1)
    Serial.print("Read EFUSE: ");
    Serial.print(read_efuse, HEX);
    Serial.print("\n");
  #endif  
  Serial.println("Burn complete."); 
  Serial.print("\n");
  Serial.println("It is now safe to remove the target AVR.");
  Serial.print("\n");
  //endSerial();
  
  // All done, disable outputs
  DATAD = 0x00;
  DATA = 0x00;
  delay(1);
  digitalWrite(RST, LOW);
  delay(1);
  digitalWrite(OE, LOW);
  digitalWrite(WR, LOW);
  digitalWrite(PAGEL, LOW);
  digitalWrite(XA1, LOW);
  digitalWrite(XA0, LOW);
  digitalWrite(BS1, LOW);
  digitalWrite(BS2, LOW);
  digitalWrite(VCC, LOW);

}

void send_cmd(byte command)  // Send command to target AVR
{
  DATAD = 0xFF;  // Set all DATA lines to outputs
  
  // Set controls for command mode
  digitalWrite(XA1, HIGH);
  digitalWrite(XA0, LOW);
  digitalWrite(BS1, LOW);

  DATA = command;
  digitalWrite(XTAL1, HIGH);  // pulse XTAL to send command to target
  delay(1);
  digitalWrite(XTAL1, LOW);
  delay(1);
}

void fuse_burn(byte fuse, int select)  // write high or low fuse to AVR
{
  // if highbyte = true, then we program HFUSE, otherwise LFUSE
  
  send_cmd(B01000000);  // Send command to enable fuse programming mode
  
  // Enable data loading
  digitalWrite(XA1, LOW);
  digitalWrite(XA0, HIGH);
  delay(1);
  
  // Load fuse value into target
  DATA = fuse;
  digitalWrite(XTAL1, HIGH);  // load data byte
  delay(1);
  digitalWrite(XTAL1, LOW);
  delay(1);
  // Decide which fuse location to burn
  switch (select) { 
  case HFUSE_SEL:
    digitalWrite(BS1, HIGH); // program HFUSE
    digitalWrite(BS2, LOW);
    break;
  case LFUSE_SEL:
    digitalWrite(BS1, LOW);  // program LFUSE
    digitalWrite(BS2, LOW);
    break;
  case EFUSE_SEL:
    digitalWrite(BS1, LOW);  // program EFUSE
    digitalWrite(BS2, HIGH);
    break;
  }
  delay(1);
   // Burn the fuse
  digitalWrite(WR, LOW); 
  delay(1);
  digitalWrite(WR, HIGH);
  delay(100);
  
  // Reset control lines to original state
  digitalWrite(BS1, LOW);
  digitalWrite(BS2, LOW);
  
}

byte fuse_read(int select) {
  byte fuse;
  
  send_cmd(B00000100);  // Send command to read fuse bits

  DATAD = 0x00;  // Set all DATA lines to inputs
  DATA = 0x00;
  digitalWrite(OE, LOW);  // turn on target output enable so we can read fuses
  
  // Set control lines and read fuses
  switch (select) {
  case LFUSE_SEL:  
    // Read LFUSE
    digitalWrite(BS2, LOW);
    digitalWrite(BS1, LOW);
    delay(1);
    fuse = DATAIN;
    break;
  case HFUSE_SEL:
    // Read HFUSE
    digitalWrite(BS2, HIGH);
    digitalWrite(BS1, HIGH);
    delay(1);
    fuse = DATAIN;
    break;
  case EFUSE_SEL:
    // Read EFUSE
    digitalWrite(BS2, HIGH);
    digitalWrite(BS1, LOW);
    delay(1);
    fuse = DATAIN;
    break;
  }
  
  digitalWrite(OE, HIGH);  // Done reading, disable output enable line
  
  return fuse;
}

byte fuse_ask(void) {  // get desired fuse value from the user (via the serial port)
  byte incomingByte = 0;
  byte fuse;
  char serbuffer[2];
  
  while (incomingByte != 'x') {  // crude way to wait for a hex string to come in
    while (Serial.available() == 0);   // wait for a character to come in
    incomingByte = Serial.read();
  }
  
  // Hopefully the next two characters form a hex byte.  If not, we're hosed.
  while (Serial.available() == 0);   // wait for character
  serbuffer[0] = Serial.read();      // get high byte of fuse value
  while (Serial.available() == 0);   // wait for character
  serbuffer[1] = Serial.read();      // get low byte
  
  fuse = hex2dec(serbuffer[1]) + hex2dec(serbuffer[0]) * 16;
  
  Serial.println(fuse, HEX);  // echo fuse value back to the user
  
  return fuse;
  
}

int hex2dec(byte c) { // converts one HEX character into a number
  if (c >= '0' && c <= '9') {
    return c - '0';
  }
  else if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  } else if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  }
}

void endSerial(void) {  // Disable serial communications
#if defined(__AVR_ATmega8__)
  // this is based on wiring's beginSerial function, it just clears
  // the serial enables and RX interrupt so we can use the port for other stuff
  
  // disable rx and tx
  UCSRB &= ~(1 << RXEN);
  UCSRB &= ~(1 << TXEN);
	
  // disable interrupt on complete reception of a byte
  UCSRB &= ~(1 << RXCIE);
  
#else	
  // disable rx and tx
  UCSR0B &= ~(1 << RXEN0);
  UCSR0B &= ~(1 << TXEN0);
	
  // disable interrupt on complete reception of a byte
  UCSR0B &= ~(1 << RXCIE0);
#endif
}
