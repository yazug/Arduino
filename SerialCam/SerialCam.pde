#include "SoftwareSerial.h"
#include "Monitor.h"

#include "uartcam_c328.h"

SoftwareSerial nssport(2,3);
UARTCamC328 cam(4,5);
Monitor term(Serial);
Monitor gps(nssport);

char buffer[128];

void setup() {
  boolean res1;
  int i;

  pinMode(13,OUTPUT);

  term.begin(38400);
  term << "Hi." << Monitor::endl;

  //  pinMode(2,INPUT);
  //  pinMode(3,OUTPUT);
  gps.begin(4800);

  
  cam.init();
  term << "inited." << term.endl;
  if ( !(i < 60) )
    term << "error!";
  delay(250);
}

void loop() {
  byte buf[8];

  cam.send(cam.CMD_INITIAL);
  delay(5);
  cam.wait(cam.CMD_INITIAL, buf, 15);
  cam.flush();
//  term.printBytes(buf, 6);
//  term << term.endl;
  
  cam.send(cam.CMD_SNAPSHOT,0x01);
  delay(5);
  cam.wait(cam.CMD_SNAPSHOT, buf, 15);
  term.printBytes(buf, 6);

  cam.send(cam.CMD_GETPICTURE, 0x01);
  delay(5);
  cam.wait(cam.CMD_GETPICTURE, buf, 15);
  delay(5);
 
   cam.wait(cam.CMD_DATA, buf, 15);
  delay(5);
  term.printBytes(buf, 4);
  term.println();
  /*
  int len = buf[0];
   len = (buf[1]<<8) | (buf[2]<<8);
   term << len << term.endl;
   for(int i = 0; i < len/8; i++) {
    cam.read(buf, 8, 25);
    term.printBytes(buf, 8);
    term.println();
   }*/
  while (true);
}







