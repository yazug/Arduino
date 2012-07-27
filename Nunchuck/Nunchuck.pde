#define TWI_FREQ 400000L

#include <Wire.h>
#include <string.h>

//#undef int
#include <stdio.h>

byte outbuf[6];		// array to store arduino output
int cnt = 0;
int ledPin = 13;

void setup () {
  Serial.begin(19200);
  Serial.println("Finished setup");
  Wire.begin();		// join i2c bus with address 0x52
  nunchuck_init (); // send the initilization handshake
}

void nunchuck_init() {
  Wire.beginTransmission (0x52);	// transmit to device 0x52
  Wire.send (0x40);		// sends memory address
  Wire.send (0x00);		// sends sent a zero.  
  Wire.endTransmission ();	// stop transmitting
}

void send_zero() {
  Wire.beginTransmission (0x52);	// transmit to device 0x52
  Wire.send (0x00);		// sends one byte
  Wire.endTransmission ();	// stop transmitting
}

void loop() {
  Wire.requestFrom (0x52, 6);	// request data from nunchuck
  while (Wire.available ()) {
      outbuf[cnt] = nunchuk_decode_byte (Wire.receive ());	// receive byte as an integer
      digitalWrite (ledPin, HIGH);	// sets the LED on
      cnt++;
    }

  // If we recieved the 6 bytes, then go print them
  if (cnt >= 5) {
      print ();
    }

  cnt = 0;
  send_zero (); // send the request for next bytes
  delay (100);
}

// Print the input data we have recieved
// accel data is 10 bits long
// so we read 8 bits, then we have to add
// on the last 2 bits.  That is why I
// multiply them by 2 * 2
void print() {
  int joy_x_axis = outbuf[0];
  int joy_y_axis = outbuf[1];
  int accel_x_axis = ((int)outbuf[2])<<2; 
  int accel_y_axis = ((int)outbuf[3])<<2;
  int accel_z_axis = ((int)outbuf[4])<<2;

  int z_button = 0;
  int c_button = 0;

 // byte outbuf[5] contains bits for z and c buttons
 // it also contains the least significant bits for the accelerometer data
 // so we have to check each bit of byte outbuf[5]
   z_button = (outbuf[5] >> 0) & 1;
   c_button = (outbuf[5] >> 1) & 1;

   accel_x_axis += (outbuf[5] >> 2) & 0b11;
   accel_y_axis += (outbuf[5] >> 4) & 0b11;
   accel_z_axis += (outbuf[5] >> 6) & 0b11;

  Serial.print(joy_x_axis, DEC);
  Serial.print("\t");

  Serial.print(joy_y_axis, DEC);
  Serial.print("\t");

  Serial.print(accel_x_axis, DEC);
  Serial.print("\t");

  Serial.print(accel_y_axis, DEC);
  Serial.print("\t");

  Serial.print(accel_z_axis, DEC);
  Serial.print("\t");

  Serial.print(z_button, DEC);
  Serial.print("\t");

  Serial.print(c_button, DEC);
  Serial.print("\t");

  Serial.println();
}

// Encode data to format that most wiimote drivers except
// only needed if you use one of the regular wiimote drivers
char nunchuk_decode_byte (char x) {
  x = (x ^ 0x17) + 0x17;
  return x;
}

