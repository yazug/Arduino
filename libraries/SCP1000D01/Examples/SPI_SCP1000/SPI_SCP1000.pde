#include <SPI.h>
#include <SCP1000D01.h>
// define spi bus pins
#define CSPIN 10
#define DRDYPIN 9

SCP1000 barometer = SCP1000(CSPIN);
unsigned char tmpbyte;	    

void setup() {
  // rising -> falling (CPOL=0), sample on leading (CPHA = 0)
  // 500kHz is maximum for SCP1000-D01
  SPI.begin();
  //delay(100);
  
  barometer.init();

  Serial.begin(9600);
  
  
  Serial.print("ASIC revision ");
  tmpbyte = barometer.revision();
  Serial.println(tmpbyte, HEX);

  delay(1000);

  Serial.print("Status Register ");
  tmpbyte = barometer.status();
  Serial.println(tmpbyte, BIN);
  if ( barometer.startup() )
    Serial.println("start-up finished.");
  else 
    Serial.println("start-up procdure running.");

  Serial.println("Select High Res. Mode");
//  write_register(OPERATION,0x0a);
  barometer.run();
//  while (digitalRead(DRDYPIN) == LOW);

}

void loop(){  
  Serial.println(barometer.temperature());
  Serial.println(barometer.pressure());
  /*
  int temp_in;
  byte tmp8;
  word tmp16;
  unsigned long pressure;

  tmp8 = read_register(DATARD8);
  tmp16 = read_register16(DATARD16);
 // tmp8 &= 0x07;

  Serial.print("PRESSURE ");
  Serial.print(tmp8, HEX);
  Serial.print(" ");
  Serial.print(tmp16, HEX);
  pressure = ((unsigned long) tmp8 << 16) | tmp16;
  Serial.print(" ");
  Serial.println(pressure, HEX);
  Serial.print(0.0025*pressure);
  Serial.println(" hPa ");

  temp_in = read_register16(TEMPOUT);
  if (temp_in & 0b100000000000000) {
    temp_in = (~temp_in & 0b0001111111111111);
    temp_in = -temp_in;
  }
  //  temp_in = abs(temp_in)>>1;
  //  temp_in = ((1.8) *temp_in) + 32;
  //  lcd.setCursor(2,1);
  //  lcd.print("TEMP F [");
  Serial.print((float)temp_in/20, 1);
  //  lcd.println("]");
  */
  Serial.println();

  delay(5000);

}


byte read_register(byte register_name) {
  byte in_byte;

  register_name <<= 2;
  register_name &= 0b11111100; //Read command

  digitalWrite(CSPIN, LOW);

  in_byte = SPI.transfer(register_name);
  in_byte = SPI.transfer(0);

  digitalWrite(CSPIN, HIGH);

  return in_byte;  
}

word read_register16(byte register_name) {
  union {
    byte u8[2];
    word u16;
  } 
  incom;

  register_name <<= 2;
  register_name &= 0b11111100; //Read command

  digitalWrite(CSPIN, LOW);

  SPI.transfer(register_name);
  incom.u8[1] = SPI.transfer(0);
  incom.u8[0] = SPI.transfer(0);

  digitalWrite(CSPIN, HIGH);

  return (incom.u16);
}

void write_register(byte register_name, byte register_value) {
  byte in_byte;

  register_name <<= 2;
  register_name |= 0b00000010; //Write command

  digitalWrite(CSPIN, LOW);

  in_byte = SPI.transfer(register_name);
  in_byte = SPI.transfer(register_value);

  digitalWrite(CSPIN, HIGH);

} 






