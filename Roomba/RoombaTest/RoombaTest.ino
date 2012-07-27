#include <SoftwareSerial.h>
#include <Roomba.h>
#include <Monitor.h>
#include <Wire.h>
#include <i2cLCD_ST7032i.h>

SoftwareSerial serport(2,3);
RoombaOI roomba(serport,5);
Monitor mon(Serial);

i2cLCD lcd = i2cLCD();

void setup() {
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);

  mon.begin(19200);
  roomba.begin();
  digitalWrite(10, HIGH);
  mon << "Awaking roomba. " << Monitor::endl;  
/*
  Wire.begin();
  lcd.begin();
  lcd.print("Awaking...");
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
*/  
  digitalWrite(10, LOW);
  mon << "Starting. " << Monitor::endl;  
  roomba.start();
  mon << roomba.OImode() << mon.endl;
//  lcd.home(); lcd.print("Start.");
  mon << "Battery voltage ";
  mon.print(roomba.voltage(), HEX);
 mon << " mV." << mon.endl;

  roomba.safe();
//  while ( roomba.OImode() != roomba.Mode_Safe )
    delay(500);
  mon << roomba.OImode() << mon.endl;
//  lcd.home(); lcd.print("Safe.");
  /*
  byte Mario_theme[] = {
    76, 16,
    76, 16,
    0, 16,
    76, 16,
    0, 16,
    72, 16,
    76, 32,
    79, 32,
    0, 32,
    67, 32,
    0, 32
  }; 

//  roomba.setSong(1,Mario_theme,2);
//  roomba.playSong(1);
*/
  //  roomba.goBackward();
  //   delay(3000);
  /*
   roomba.spinLeft();
   delay(1000);
   */
  //   mon << "Slowly..." << Monitor::endl;  
  //   roomba.goSlowly();

  digitalWrite(10, HIGH);
}

void loop() {
  byte bw = roomba.bumpsAndWheelDrops();
  byte cw = roomba.cliffsAndWalls();
  int d, a;
  roomba.getDistanceAngle(d, a);
  word ws = roomba.wallSignal();
  byte irc = roomba.IRCharacterOmni();
  word irlr = roomba.IRCharacterLeftRight();
  byte lb = roomba.lightBumper();
  word lbs[6];
  roomba.getLightBumpSignals(lbs);
  word lec = roomba.leftEncoderCounts();
  word rec = roomba.rightEncoderCounts();

  mon << (millis()/1000) << Monitor::endl;
  mon << "#Bump L , R,  Wheel Drop L, R,  Cliff L, FL, FR, R" << mon.endl
    << roomba.bumpLeft(bw) << ", " << roomba.bumpRight(bw) << ",  "
    << roomba.wdropLeft(bw) << ", " << roomba.wdropRight(bw) 
    << ",  " << roomba.cliffLeft(cw) << ", " << roomba.cliffFrontLeft(cw)
      << ", " << roomba.cliffFrontRight(cw) << ", " << roomba.cliffRight(cw) << mon.endl;
  mon << "#Wall, Wall Signal, Virtual Wall" << mon.endl
    << roomba.wall(cw) << ", " << ws << ",  " << roomba.virtualWall(cw) << mon.endl;
  mon << "#L.Bumper L, F.L, C.L, C.R, F.R,  L.Bumper signals" << mon.endl;
  mon << roomba.lightBumperLeft(lb) << ", " << roomba.lightBumperFrontLeft(lb) << ", " << roomba.lightBumperCenterLeft(lb) 
    << ", " << roomba.lightBumperCenterRight(lb) << ", " << roomba.lightBumperFrontRight(lb) << ", " << roomba.lightBumperRight(lb) << ",  ";
  for (int i = 0; i < 6; i++) {
    mon << lbs[i] << ", ";
  }
  mon << mon.endl;
  mon << "#IR Character Omni, L, R,  Encoder counts left, right,  Trip distance, angle" << mon.endl;
 mon << irc << ", " << highByte(irlr) << ", " << lowByte(irlr) << ", "
    << lec << ", " << rec << ", " << d << ", " << a << mon.endl;
  mon << Monitor::endl;
  delay(1000);
}





