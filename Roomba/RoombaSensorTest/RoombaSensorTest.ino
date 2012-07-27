#include <SoftwareSerial.h>
#include <Roomba500.h>
#include <Monitor.h>

SoftwareSerial serport(2,3);
Roomba500 roomba(serport,5);
Monitor mon(Serial);

long distance, angle;

void setup() {
  mon.begin(19200);

  mon << "Awaking roomba. " << Monitor::endl;
  roomba.begin(19200);

  mon << "Starting. " << Monitor::endl;  
  roomba.start();
  // roomba.takeModeChangeDelay();
  mon << "In Passive mode: " << (roomba.mode() == roomba.Mode_Passive ? "yes." : "no.") << Monitor::endl;  

  mon << "Battery voltage " << roomba.voltage() << " mV." << mon.endl;

  roomba.safe();
  //  roomba.takeModeChangeDelay();
  mon << "In Safe mode: " << (roomba.mode()==roomba.Mode_Safe?"yes.":"no.") << Monitor::endl;  

  distance = 0;
  angle = 0;
}

void loop() {
  char tmp[16] = "";
  if ( mon.available() ) {
    mon.readToken(tmp);
    mon << ">> " << tmp << mon.endl;
  }
  switch (tmp[0]) {
    case 'f':
      roomba.goSlowly();
      break;
      case 'h':
      roomba.stop();
      break;
      case 'r':
      roomba.spinRight();
      break;
      case 'l':
      roomba.spinLeft();
      break;
      case 'b':
      roomba.forward();
      delay(500);
      roomba.stop();
      break;
  case 's':
  case 'S':
    byte bw = roomba.bumpsAndWheelDrops();
    byte cw = roomba.cliffsAndWalls();

    roomba.getDistanceAngle(distance, angle);
    word ws = roomba.wallSignal();
    byte iromni, irleft, irright;
    roomba.getIRCharacters(iromni, irleft, irright);
    byte lb = roomba.lightBumper();
    word lbs[6];
    roomba.getLightBumpSignals(lbs);
    word lec = roomba.leftEncoderCounts();
    word rec = roomba.rightEncoderCounts();

    mon << "Millis Clock = " << (millis()) << Monitor::endl;
    mon << "Bump L, R,  Wheel Drop L, R,  Cliff L, FL, FR, R = "
      << roomba.bumpLeft(bw) << ", " << roomba.bumpRight(bw) << ",  "
      << roomba.wdropLeft(bw) << ", " << roomba.wdropRight(bw) 
      << ",  " << roomba.cliffLeft(cw) << ", " << roomba.cliffFrontLeft(cw)
        << ", " << roomba.cliffFrontRight(cw) << ", " << roomba.cliffRight(cw) << mon.endl;
    mon << "Wall, Wall Signal, Virtual Wall = "
      << roomba.wall(cw) << ", " << ws << ",  " << roomba.virtualWall(cw) << mon.endl;
    mon << "Light Bumper L, F.L, C.L, C.R, F.R = ";
    mon << roomba.lightBumperLeft(lb) << ", " << roomba.lightBumperFrontLeft(lb) << ", " << roomba.lightBumperCenterLeft(lb) 
      << ", " << roomba.lightBumperCenterRight(lb) << ", " << roomba.lightBumperFrontRight(lb) << ", " << roomba.lightBumperRight(lb) << mon.endl;
    mon <<  "Light Bumper signals = " ;
    for (int i = 0; i < 6; i++) {
      mon << lbs[i] << ", ";
    }
    mon << mon.endl;
    mon << "IR Character Omni, L, R = " << iromni << ", " << irleft << ", " << irright << mon.endl;
    mon << "Encoder counts left, right,  Trip distance, angle = "
      << lec << ", " << rec << ", " << distance << ", " << angle << mon.endl;
    mon << Monitor::endl;
    break;
  }
}







