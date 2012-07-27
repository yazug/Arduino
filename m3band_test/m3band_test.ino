//#include <Servo.h>
//#include <SoftwareSerial.h>

Servo myservo;

struct {
  byte length;
  byte data[256];
  byte checksum;
  byte xsum;
  byte poorQuality;

  boolean connected;
  boolean poorSignal;
} 
payload;

long prevOccation;
int currPos;

float attention;

void setup() {
  Serial1.begin(57600);
  Serial.begin(19200);
  Serial.println("go.");
  Serial1.write(0xc2);

  myservo.attach(9);
  prevOccation = millis();
  currPos = 90;
}


void loop() {
  updateAttention();        // <1>
    analogWrite(5, 32+attention*200);
  if ( millis() > prevOccation+500) {
    prevOccation = millis();
    int pos = attention*128;
    pos = max(pos,0);
    pos = min(pos, 180);
    if ( currPos != pos ) {
      currPos += (pos-currPos)/2;
      myservo.write(currPos);
    }
    if ( payload.connected && !payload.poorQuality ) {
      Serial.print('+');
    } else {
      Serial.print('-');
    }
    for(int i = 0; i < (1.0 - attention)*16; i++) {
       // Serial.println(attention);
       Serial.print('*');
    }
    Serial.println();
    /*
    for(int i = 0;  i < payload.length; i++) {
     Serial.print(payload.data[i], HEX);
     Serial.print(" ");
     } 
     Serial.println();
     */
     
  }
}


float updateAttention()
{
  //  byte generatedChecksum = 0;
  //  byte checksum = 0;
  //  int payloadLength = 0;
  //  byte payloadData[256] = { 
  //    0   };
  word rval;
  int poorQuality = 0;

  while ((0xaa != getOneByte(Serial1)) && (0 < Serial.available())) {
    smartFlush();        // <6>

  }
  if (0xaa != getOneByte(Serial1)) return -1;        // <7>

  /* Length */
  payload.length = getOneByte(Serial1);
  if (payload.length > 169)
    return -2;

  /* Checksum */
  payload.xsum = 0;
  for (int i = 0; i < payload.length; i++) {
    payload.data[i] = getOneByte(Serial1); // Read payload into array
    payload.xsum += payload.data[i];
  }
  payload.checksum = getOneByte(Serial1);
  payload.xsum = 255 - payload.xsum;
  if (payload.checksum != payload.xsum) {
    return -3;
  }

  /* Analyse payload */
  for (int i = 0; i < payload.length; i++) {
    switch (payload.data[i]) {
    case 0xD0:
      payload.connected = false;
      break;
    case 4:
      setYellow(true);
      i++;
      attention = payload.data[i] / 100.0; // <8>
      payload.connected = true;
      break;
    case 2:
      i++;
      payload.poorQuality = payload.data[i];
      if (170 < payload.poorQuality) {        // max 200
        setYellow(true);
        //attention = 0.0;
        payload.connected = true;
        return -4;
      }
      break;
    case 0xD1:        // Headset Not Found
    case 0xD2:        // Headset Disconnected
    case 0xD3:        // Request Denied
    case -70:
      payload.connected = false;
      setYellow(true);
      return -5;
      break;
    case 0x80:
        payload.connected = true;
      i = i + 3;
      break;
    case 0x83:
        payload.connected = true;
      i = i + 25;
      break;
    } // switch
  } // for
}

void smartFlush()
{
  if (128 / 2 < Serial.available()) {   // buffer is 128 B // <9>
    Serial.flush();
  }
}


int getOneByte(Stream & port) {
  long watch = millis();
  while (! port.available() ) {
    if ( millis() > watch + 50 ) 
      return -1;
  }
  return port.read();
}

void headsetConnected(boolean s) {
  pinMode(13, OUTPUT);
  if ( s ) {
    digitalWrite(13, HIGH);
  } 
  else {
    digitalWrite(13, HIGH);
    delay(5);
    digitalWrite(13, LOW);
  }
}


void setYellow(boolean s)
{
  pinMode(5, OUTPUT);
  if ( s )
    digitalWrite(5, HIGH);
  else digitalWrite(5, LOW);
}



