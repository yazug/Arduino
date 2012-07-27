#include <CameraC328R.h>
#include <SoftwareSerial.h>

#define LED_PIN 13
#define PAGE_SIZE 64
#define USB_BAUD 57600
#define CAMERA_BAUD 28800

SoftwareSerial myport(4, 5);
CameraC328R camera(myport);

uint16_t pictureSizeCount = 0;

void setup() {
  Serial.begin( USB_BAUD );
  myport.begin(CAMERA_BAUD);
//  camera.begin(CAMERA_BAUD);
  pinMode( LED_PIN, OUTPUT );
  digitalWrite( LED_PIN, LOW );
}

void loop() {
  if( Serial.available() ){

    while(Serial.read() != -1);

    digitalWrite( LED_PIN, HIGH );

    if( !camera.sync() ) {
      Serial.println( "Sync failed." );
      return;
    }

    if( !camera.initial( CameraC328R::CT_JPEG, CameraC328R::PR_160x120, CameraC328R::JR_320x240 ) ) {
      Serial.println( "Initial failed." );
      return;
    }

    if( !camera.setPackageSize( 64 ) ) {
      Serial.println( "Package size failed." );
      return;
    }

    if( !camera.setLightFrequency( CameraC328R::FT_60Hz ) ) {
      Serial.println( "Light frequency failed." );
      return;
    }

    if( !camera.snapshot( CameraC328R::ST_COMPRESSED, 0 ) ) {
      Serial.println( "Snapshot failed." );
      return;
    }

    pictureSizeCount = 0;
    if( !camera.printJPEGPicture(Serial, CameraC328R::PT_JPEG, PROCESS_DELAY) ) {
      Serial.println( "Get JPEG failed." );
      return;
    }
  }
}

