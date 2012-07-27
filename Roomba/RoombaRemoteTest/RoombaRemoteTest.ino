#include <SoftwareSerial.h>
#include <Roomba500.h>
#include <Monitor.h>

SoftwareSerial serport(2,3);
Roomba500 roomba(serport,5);
Monitor mon(Serial);

const int LED_monitor = 10;
inline void flashLED() {
  digitalWrite(LED_monitor, LOW);
  delay(10);
  digitalWrite(LED_monitor, HIGH);
}

long distance, angle;

void setup() {
  pinMode(LED_monitor, OUTPUT);
  digitalWrite(10, HIGH);
  mon.begin(19200);

  flashLED();
  mon << "Awaking roomba. " << Monitor::endl;
  roomba.begin(19200);

  mon << "Starting. " << Monitor::endl;  
  roomba.start();
  mon << "In mode id = "<< roomba.mode() << Monitor::endl;  
  mon << "Battery voltage = " << roomba.voltage()/1000.0 << " V." << mon.endl;

  //  roomba.takeModeChangeDelay();
  mon << "In mode id = " << roomba.mode()<< Monitor::endl;  

  distance = 0;
  angle = 0;
}

void loop() {
  int velocity = 0;
  char buf[16] = "";
  char cmd;

  if ( mon.available() and mon.readToken(buf) != 0 ) {
    cmd = buf[0];
    if ( strlen(buf) > 1 ) {
      velocity = atoi(buf+1);
      if ( velocity > 500 or velocity < -500 )
        velocity = 0;
    } 
    else {
      velocity = 120;
    }
    flashLED();
    switch(cmd) {
    case '0':
      roomba.power();
      mon << "Power button." << mon.endl;
      break;
    case '2':
      roomba.safe();
      mon << "In mode id = "<< roomba.mode() << mon.endl;
      break;
    case 'd':
      roomba.seekDock();
      mon << "Seek dock." << mon.endl;
      break;
    case 'f':
      roomba.goForward(velocity);
      mon << "forward "<< velocity << mon.endl;
      break;
    case 'b':
      roomba.goBackward(velocity);
      mon << "back "<< velocity << mon.endl;
      break;
    case 'r':
      roomba.spinRight(velocity);
      mon << "right "<< velocity << mon.endl;
      break;
    case 'l':
      roomba.spinLeft(velocity);
      mon << "spin left "<< velocity << mon.endl;
      break;
    case 's':
      roomba.stop();
      mon << "stop. " << mon.endl;
      break;
    case 'q':
      printSensorValues();
      break;
    case 'p':
      play();
      break;
    }
  }
}

void play() {
  //    roomba.send( 128); // start
  //    roomba.send(130); // control
  //Marioのテーマ
  char mario[] = {
    76, 12,
    76, 12,
    0, 12,
    76, 12,
    0, 12,
    72, 12,
    76, 24,
    79, 24,
    0, 24,
    67, 24,
    0, 24,
    0, 0,
  };

  char down[] = {
    71, 12,
    77, 12,
    0, 12,
    77, 12,
    77, 16,
    76, 16,
    74, 16,
    72, 12,
    0,  12,
      67, 12,
      0, 12,
    60, 12,
    0, 0,
  };
  roomba.setSong(0, mario, 11);
  roomba.setSong(1, down, 12);
  delay(500);
  roomba.playSong(0);
  while ( roomba.songPlaying() );
  
  roomba.playSong(1);
  mon << ( roomba.songPlaying() ? "yes" : "no" ) << mon.endl;
  
  /*
  mon << " Play song 1" << mon.endl;     
   roomba.send(141); //play 
   roomba.send(1);//
   */
  delay(1000);

}

void printSensorValues() {

  byte bw = roomba.bumpsAndWheelDrops();
  byte cw = roomba.cliffsAndWalls();

  distance += roomba.distance();
  angle += roomba.angle();

  word ws = roomba.wallSignal();
  byte iromni, irleft, irright;
  roomba.getIRCharacters(iromni, irleft, irright);
  byte lb = roomba.lightBumper();
  word lbs[6];
  roomba.getLightBumpSignals(lbs);
  word lec = roomba.leftEncoderCounts();
  word rec = roomba.rightEncoderCounts();

  mon << "Millis Clock = " << (millis()) << Monitor::endl;
  mon << "Bump L " << roomba.bumpLeft(bw) << ", R " << roomba.bumpRight(bw) 
    << ",  Wheel Drop L " << roomba.wdropLeft(bw) << ", R " << roomba.wdropRight(bw) 
      << ",  Cliff L " << roomba.cliffLeft(cw) << ", FL " << roomba.cliffFrontLeft(cw) 
        << ", FR " << roomba.cliffFrontRight(cw) << ", R " << roomba.cliffRight(cw) 
          << mon.endl;
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
  mon << "Charging state = " << roomba.chargingState() << mon.endl;
  mon << "IR Character Omni, L, R = " << iromni << ", " << irleft << ", " << irright << mon.endl;
  mon << "Encoder counts left, right,  Trip distance, angle = "
    << lec << ", " << rec << ", " << distance << ", " << angle << mon.endl;
  mon << Monitor::endl;
}















