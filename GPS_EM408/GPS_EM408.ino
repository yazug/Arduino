#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <SoftwareSerial.h>

SoftwareSerial serport(2,3);
const int bufferSize = 128;
char buffer[bufferSize], * bp;
int pos, head;
boolean lineFinished;

union datatoken {
  char str[12];
  long val;
} data[16];

void setup() {
  serport.begin(4800);
  Serial.begin(19200);
  //
  lineFinished = true;
  pos = head = 0;
}

void loop() {
  if ( serport.available() ) {
    int c;
    while ( serport.available() ) {
      c = serport.read();
      buffer[pos] = (char) c;
      if ( (char)c == '$' ) {
        head = pos;
      } 
      else if ( (char) c == '\n' ) {
        buffer[pos] = 0;

        if ( compareToBufferPrefix("$GPGSV") ) {
          Serial.println("GPS Satellites in view");
        } 
        else if ( compareToBufferPrefix("$GPGSA") ) {
          Serial.println("GPS DOP and active satellites");
        } 
        else if ( compareToBufferPrefix("$GPGLL") ) {
          Serial.println("Geographic position, latitude / longitude");
          printBuffer();
          Serial.println();
          Serial.println();
        } 
        else if ( compareToBufferPrefix("$GPGGA") ) {
          parse();
          Serial.print(data[0].str);
          Serial.print(" T");
          Serial.print(data[1].val);
          Serial.print(" E");
          Serial.print(data[2].val);
          Serial.print(" N");
          Serial.print(data[4].val);
          Serial.print(" M");
          Serial.print(data[9].val);
          Serial.println();
          Serial.println("Global Positioning System Fix Data");
          printBuffer();
          Serial.println();
          Serial.println();
        } 
        else if ( compareToBufferPrefix("$GPRMC") ) {
          Serial.println("Recommended minimum specific GPS/Transit data");
          printBuffer();
          Serial.println();
          Serial.println();
        } 
        else if ( compareToBufferPrefix("$GPHDT") ) {
          Serial.println("Heading, True");
        } 
        else {
          Serial.println("?????!!!!!");
          printBuffer();
        }
      }
      pos++;
      pos %= bufferSize;
    }
  }
}

boolean compareToBufferPrefix(const char * pattn) {
  int i;
  char t;
  for(i = 0; pattn[i] != 0 && (t = buffer[(head+i)%bufferSize]) != 0; i++) {
    if ( pattn[i] != t ) break;
  }
  return pattn[i] == 0;
}

size_t printBuffer() {
  int i;
  for (i = 0; i < bufferSize ; i++) {
    char t = (char) buffer[(head+i) % bufferSize];
    if ( t == 0 ) break;
    Serial.print(t);
  }
  return i;
}

void parse() {
  char tmp[8];
  int i;
  int tnum = 0;
  int thead = 0;
  i = 0; 
  while ( i < bufferSize && isprint(buffer[(head+i) % bufferSize]) ) {
    char c = buffer[(head+i) % bufferSize];
    if ( c == ',' ) {
      strncpy(tmp, buffer+((head+thead+1) % bufferSize), i-thead-1);
      tmp[i-thead-1] = 0;
      if ( tmp[0] == 'G' ) {
        strcpy(data[tnum].str, tmp);
      } else if ( isdigit(tmp[0]) ) {
        data[tnum].val = (long)(atof(tmp)*100); 
      } else if ( isalpha(tmp[0]) ) {
        strcpy(data[tnum].str,tmp);
      } else if ( tmp[0] == 0 ) {
        data[tnum].str[0] = 0;
      }
      /*
      Serial.print(tnum);
      Serial.print(": \"");
      Serial.print(tmp);
      Serial.print("\", ");
      */
      tnum++;
      thead = i;
    }
    i++;
  }
  Serial.println();
}

