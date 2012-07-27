import processing.serial.*;

Serial myPort;
String filename = "photo.jpg";
byte[] photo = {
};
Boolean readData = false;
PImage captureImage;

void setup()
{
  size(320,240);
  println( Serial.list() );
  myPort = new Serial( this, Serial.list()[1], 57600 ); //Serial.list()[3]は環境に合わせて変更すること。
}

void draw()
{
  byte[] buffer = new byte[64];
  if( readData )
  {
    if ( myPort.available() > 0 ) {
      print("Reading bytes");
      while( myPort.available() > 0 )
      {
        int readBytes = myPort.readBytes( buffer );
        //print( readBytes );
        print( "." );
        for( int i = 0; i < readBytes; i++ )
        {
          photo = append( photo, buffer[i] );
        }
        long ctime = millis();
        while ( myPort.available() == 0 && (millis() < ctime + 24));
      }
      println();
    }
  }
  else
  {
    while( myPort.available() > 0 )
    {
      print( "COM Data: " );
      println( myPort.readString() );
    }
  }
}

void keyPressed()
{
  if( photo.length > 0 ) {
    readData = false;
    println();
    print( "Writing to disk " );
    print( photo.length );
    println( " bytes ..." );
    saveBytes( filename, photo );
    println( "DONE!" );
    photo = new byte[0];

    captureImage = loadImage(filename);
    image(captureImage, 0, 0);
  }
  else {
    readData = true;
    myPort.write(0);
    println( "Reading data ..." );
  }
}


