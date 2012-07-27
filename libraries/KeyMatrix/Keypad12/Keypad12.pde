#include "Monitor.h"

struct KeyPad {
  boolean buttonDown;
  const int * row_pins, * col_pins;
  int row_size, column_size;
  const char * keytops;

  KeyPad(const int rows[], const int rsize, const int cols[], const int csize, const char keys[]) { 
    row_pins = rows;
    row_size = rsize;
    col_pins = cols;
    column_size = csize;
    keytops = keys;
    buttonDown = false; 
  }

  char scan_Keypad() {
    long swatch;
    boolean currentBState = false;
    int r, c;
    for (r = 0; r < row_size; r++) {
      pinMode(row_pins[r], INPUT);
      digitalWrite(row_pins[r], HIGH);
      for(c= 0; c < column_size; c++) {
        pinMode(col_pins[c], OUTPUT);
        digitalWrite(col_pins[c], LOW);
        if ( digitalRead(row_pins[r]) == LOW ) {
          swatch = millis();
          while ( digitalRead(row_pins[r]) == LOW ) {
            if ( millis() - swatch > 5 or millis() < swatch) break;
          }
          if ( millis() - swatch > 5 or millis() < swatch )
            currentBState = true;
        }
        digitalWrite(col_pins[c],HIGH);
        if ( currentBState )
          goto finish_keyscans;
      }    
    }
finish_keyscans:
    if ( currentBState && buttonDown) 
      return keytops[row_size*column_size];
    if ( currentBState && !buttonDown) {
      buttonDown = true;
      return keytops[r*column_size + c];
    }
    if ( !currentBState ) {
      buttonDown = false;
      return keytops[r*column_size + c];
    }      
  }

};

const int rows[] = { 
  4,5,6,7,3 };
const int columns[] = { 
  16,15,14   };
const char ktops[] = { 
  '1','2','3', '4','5','6', '7','8','9', '<','0','>','?','?','E',0   };

KeyPad keypad(rows, 5, columns, 3, ktops);

long value;
char message[10];

Monitor mon(Serial);

void setup() {
  Serial.begin(9600);
  mon << "Hello." << mon.endl;
}

void loop() {
  char t = keypad.scan_Keypad();
  if ( t != 0 ) {
    mon << t << mon.endl;
  }
}



