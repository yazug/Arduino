/*
 * Kent LCD Example code
 * Copyright (c) 2010 SparkFun Electronics.  All right reserved.
 * Written by Chris Taylor
 *
 * This code was written to demonstrate Large (320x240) Kent LCD Display 
 * 
 * This code will print vertical bars on the Kent LCD
 * Note: For this code to work properly with the Small Kent display, change
 * the loop max value in line 55. All other procedures work for both displays.
 *
 * http://www.sparkfun.com
 */

#define DATAOUT 11//MOSI
#define DATAIN  12//MISO 
#define SPICLOCK  13//sck
#define SLAVESELECT 10//ss

char clr = 0;

void setup()
{
  Serial.begin(9600); // For debugging

  pinMode(DATAOUT, OUTPUT);  // Init SPI
  pinMode(DATAIN, INPUT);
  digitalWrite(DATAIN,LOW);
  pinMode(SPICLOCK,OUTPUT);
  pinMode(SLAVESELECT,OUTPUT);
  digitalWrite(SLAVESELECT,HIGH); //disable device 

  Serial.println("Kent LCD Test");
}

void loop()
{
  // Initialize SPI communication
  SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR1) | (1<<SPR0) | (1<<CPHA);
  clr=SPSR;
  clr=SPDR;

  // Send data to onboard RAM
  for(int i = 0x00; i < 0x25; i+=0x01){ // 37.5 blocks of 256 bytes (last 128 bytes sent after this loop)
    select();
    spi_transfer(0x00); // TX command
    spi_transfer(i); // Address high byte
    spi_transfer(0x00); // Address low byte
    for(int j = 0; j < 256; j++){ // Data
      if(j%2) { 
        spi_transfer(0xFF); 
      }
      else{ 
        spi_transfer(0x00); 
      }
    }
    deselect();
    delay(5); // Or you could poll the BUSY pin, whatever floats your boat
  }
  // Send the last 128 bytes
  select(); 
  spi_transfer(0x00);
  spi_transfer(0x25);
  spi_transfer(0x00);
  for(int j = 0; j < 128; j++){
    spi_transfer(0x00);
  }
  deselect();
  delay(5);

  Serial.println("Data transfer complete.");

  select(); // Screen update
  spi_transfer(0x18);
  spi_transfer(0x00);
  spi_transfer(0x00);
  deselect();

  Serial.println("Screen update complete.");

  while(1);
}

void select() {
  digitalWrite(SLAVESELECT,LOW);
}

void deselect() {
  digitalWrite(SLAVESELECT,HIGH);
}

char spi_transfer(volatile char data)
{
  SPDR = data;                    // Start the transmission
  while (!(SPSR & (1<<SPIF)))     // Wait for the end of the transmission
  {
  };
  return SPDR;                    // return the received byte
}


