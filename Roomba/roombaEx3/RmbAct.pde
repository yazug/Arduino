
void goForward() {
  Serial1.print((byte) 137);   // DRIVE
  Serial1.print((byte) 0x01);   // 0x012c == 300? mm/sec
  Serial1.print((byte) 0x2c);
  Serial1.print((byte) 0x80);
  Serial1.print((byte) 0x00);
}
void slowly(){
  Serial1.print((byte) 137);   // DRIVE
  Serial1.print((byte) 0x00);  // 0x00c8 = 200
  Serial1.print((byte) 0xc8);
  Serial1.print((byte) 0x80);
  Serial1.print((byte) 0x00);
}
void goBackward() {
  Serial1.print((byte) 137);   // DRIVE
  Serial1.print((byte) 0xff);   // 0xff38 == -200
  Serial1.print((byte) 0x38);
  Serial1.print((byte) 0x80);
  Serial1.print((byte) 0x00);
}
void spinLeft() {
  Serial1.print((byte) 137);   // DRIVE
  Serial1.print((byte) 0x00);   // 0x0064 == 100
  Serial1.print((byte) 0x64);
  Serial1.print((byte) 0x00);
  Serial1.print((byte) 0x01);   // 0x0001 == spin left
}
void spinRight() {
  Serial1.print((byte) 137);   // DRIVE
  Serial1.print((byte) 0x00);   // 0x0064 == 100
  Serial1.print((byte) 0x64);
  Serial1.print((byte) 0xff);
  Serial1.print((byte) 0xff);   // 0xffff == -1 == spin right
}
void turnLeft() {
  Serial1.print((byte) 137);   // DRIVE
  Serial1.print((byte) 0x00);   //
  Serial1.print((byte) 0xfa);
  Serial1.print((byte) 0x00);
  Serial1.print((byte) 0x81);   //
}
void turnRight() {
  Serial1.print((byte) 137);   // DRIVE
  Serial1.print((byte) 0x00);   // 0x00fa == 250
  Serial1.print((byte) 0xfa);
  Serial1.print((byte) 0xff);
  Serial1.print((byte) 0x7f);   //
}
void Rstop(){
  Serial1.print((byte) 137); // drive
  Serial1.print((byte) 0x00); // speed
  Serial1.print((byte) 0x00); //stop
  Serial1.print((byte) 0x80); //  
  Serial1.print((byte) 0x00); // 
}

void Clean(){
 	if(roomba != CLEAN){
 		Serial1.print((byte) 135); // clean
 		roomba = CLEAN;
	} else{
		Serial1.print((byte) 130); // control
		lcd.clear();//lcd////////////
		roomba = NOTHING;
	}
}
int analogReadFiltered(int ch, int times) {//値を平均で出す
  unsigned int val = 0;
  for (int i = 0; i < times; i++) {
    val += analogRead(ch);
    delayMicroseconds(1333);
  }
  return val/times;
}
void upSen(){  //センサー読み取りコマンド   
  Serial1.print((byte) 142);  
  Serial1.print((byte) 0); //
  delay(100); // wait for sensors 
  char i = 0;


  while (Serial1.available()) { 
    int c = Serial1.read();
    sen[i++] = c;
  }

  value = analogReadFiltered(0, 10); //analogRead(0); //値読み取り右側
  cm = ((unsigned int)(value/2) * (unsigned int)(2.54 * 64)) >> 6 ;

  valuel =  analogReadFiltered(1, 10); //analogRead(1); //値読み取り左
  cml = ((unsigned int)(valuel/2) * (unsigned int)(2.54 * 64)) >> 6 ;

  valuet =  analogReadFiltered(2, 10); //analogRead(2); //値読み取り前方
  cmt = ((unsigned int)(valuet/2) * (unsigned int)(2.54 * 64)) >> 6 ;

}

