// sensor key
#define KEYPORT PORTB
#define KEYDDR  DDRB
#define KEYPIN  PINB
#define KEY0    PB0        // capture input - digital 8
#define KEY1    PB1        // capture input - digital 9
#define KEY2    PB2        // capture input - digital 10
#define KEY3    PB3        // capture input - digital 11
#define KEY4    PB4        // capture input - digital 12
#define KEY5    PB5        // capture input - digital 13

/*Capacitative Sensing Code for ATMega328 Arduinos*/

void setup() {

  Serial.begin(9600);      // connect to the serial port
}


// returns capacity on one input pin
// pin must be the bitmask for the pin e.g. (1<<PB0)
char getcap(char pin) {

  char i = 0;
  DDRB &= ~pin;          // input
  PORTB |= pin;          // pullup on
  for(i = 0; i < 16; i++)
    if( (PINB & pin) ) break;
  PORTB &= ~pin;         // low level
  DDRB |= pin;           // discharge
  return i;
}



void loop () {

  char capval[6];
  char pinval[6] = {1<<PINB0,1<<PINB1,1<<PINB2,1<<PINB3,1<<PINB4,1<<PINB5};
  delay(1000);
  for(char i = 0; i < 6; i++)
  {
    capval[i] = getcap(pinval[i]);
    Serial.print("digital ");
    Serial.print(i+8, DEC);
    Serial.print(": ");
    Serial.println(capval[i], DEC);
  }
  Serial.println("");
}
