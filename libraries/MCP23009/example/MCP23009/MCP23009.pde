/*
void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(1000);
  pinMode(13,OUTPUT);
  
  WriteMCPReg(ADDR, IODIR,0b11111111); // set IODIR (0x00) to 0x00, all output
  WriteMCPReg(ADDR, IOCON,0x04); // set IOCON
  WriteMCPReg(ADDR, GPPU,0xff); // set pull-ups
  delay(20);
}

 void loop() {
  unsigned char r;
  */ /*
  WriteMCPReg(ADDR, GPIO,0x03);
  digitalWrite(13,HIGH);
  delay(1000);
  WriteMCPReg(ADDR, GPIO,0x00); 
  digitalWrite(13,LOW);
  */ /*
  delay(1000);
  r = ReadMCPReg(ADDR, GPIO, r); 
  Serial.print("## ");
  Serial.println(r, BIN);
}
*/

