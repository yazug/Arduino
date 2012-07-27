/*
 *  Kent_LCD.h
 *  
 *
 *  Created by Sin on 10/06/27.
 *  Copyright 2010. All rights reserved.
 *
 */


#define KENT_WRITE	((byte)0x00)
#define KENT_FILL	0x01
#define KENT_READ	0x04

#define KENT_DISP_FULLSCRN 0x18
#define KENT_DISP_PARTSCRN 0x19

#define KENT_RESET	0x24

class KentChLCD {
  byte ssPin, busyPin;
  word pixColumns, pixRows;

public:
  KentChLCD(byte ss, byte busy = 0xff, boolean large = false) {
    ssPin = ss;
    busyPin = busy;
    if ( large ) {
      pixColumns = 320;
      pixRows = 240;
    } 
    else {
      pixColumns = 240;
      pixRows = 160;
    }
  }

  void begin() {
    SPI.begin();
    SPI.clockRate(3);  // 250kHz max
    SPI.dataMode(1);  // latch on the falling edge
  }

  word columns() {
    return pixColumns;
  }

  word rows() {
    return pixRows;
  }

  void reset() {
    SPI.select(ssPin);
    SPI.transfer(KENT_RESET); // software reset
    SPI.deselect(ssPin);
  }

  void update() {
    SPI.select(ssPin); // Screen update
    SPI.transfer(KENT_DISP_FULLSCRN);
    SPI.transfer();
    SPI.transfer();
    SPI.deselect(ssPin);
  }
  
  void clear(byte val = 0) {
    SPI.select(ssPin); // Screen update
    SPI.transfer(KENT_FILL);
    SPI.transfer((byte)0);
    SPI.transfer((byte)0);
    SPI.transfer(highByte(4799));
    SPI.transfer(lowByte(4799));
    SPI.transfer(val);
    SPI.deselect(ssPin);
  }

  void write(word r, word c, byte bmap[], word size) {
    word addr, bits16;
    byte left, right;
    addr = r*30 + c/8;
/*
    SPI.select(ssPin);
    SPI.transfer(KENT_READ);
    SPI.transfer(highByte(addr));
    SPI.transfer(lowByte(addr));
    SPI.transfer();
    SPI.transfer();
    left = SPI.transfer();
    right = SPI.transfer();
    SPI.deselect(ssPin);
    _delay_ms(5); // Or you could poll the BUSY pin, whatever floats your boat
*/
//    left &= ~highByte(mask16);
//    left |= highByte(bits16) & highByte(mask16);
//    right &= ~lowByte(mask16);
//    right |= lowByte(bits16) & lowByte(mask16);
    
    SPI.select(ssPin);
    SPI.transfer(KENT_WRITE); // TX command
    SPI.transfer(highByte(addr));
    SPI.transfer(lowByte(addr));
    for(int i = 0; i < size; i++) {
      SPI.transfer(bmap[i]);
    }
    SPI.deselect(ssPin);
    _delay_ms(5); // Or you could poll the BUSY pin, whatever floats your boat

  }
};







