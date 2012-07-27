#include "NewSoftSerial.h"

class UARTCamC328 {
  
  NewSoftSerial & ssport;

public:
  static const byte HEAD = 0xaa;
  static const byte CMD_SYNC = 0x0d;
  static const byte CMD_INITIAL = 0x01;
  static const byte CMD_GETPICTURE = 0x04;
  static const byte CMD_SNAPSHOT = 0x05;
  static const byte CMD_DATA = 0x0a;
  static const byte CMD_ACK = 0x0e;
  static const byte CMD_SETBAUDRATE = 0x07;

  UARTCamC328(SoftwareSerial & ss) : 
  ssport(ss) {
  }

  void send(const byte cmd, const byte param1 = 0, const byte param2 = 0, const byte param3 = 0, const byte param4 = 0) {
    byte msg[] = { HEAD, cmd, param1, param2, param3, param4 };
    ssport.write((const uint8_t*)msg, 6);
    delay(5);
  }

  boolean wait(const byte cmd, byte result[], const long limit) {
    int i = 0;
    byte c;
    long current = millis();
    while ( i < 6 && millis() < current + limit ) {
      if ( ssport.available() && (c = ssport.read()) != -1 ) {
        if ( i == 0 && c == HEAD ) {
        } 
        else if ( i == 1 && c == CMD_ACK ) {
        } 
        else if ( i == 2 && c == cmd ) {
        } 
        else if ( i > 2 && i < 6 ) {
        } 
        else {
          break;
        }
        result[i-2] = c;
        i++;
        current = millis();
      }
    }
    if ( i < 6 )
      return false;
    return true;
  }

  void init() {
    byte tmp[6];
    int i;
    ssport.begin(19200);
    Serial.println("a");
    for (i = 0; i < 60 ; i++ ) {
      send(CMD_SYNC);
    Serial.println("b");
      if ( (wait(CMD_ACK,tmp,15) ) 
        && wait(CMD_SYNC, tmp, 15) ) {
        send(CMD_ACK, CMD_SYNC);
        //term << "; sync/ack succeeded.";
    Serial.println("c");
        break;
      }
    Serial.println("d");
      delay(10);
    }
    delay(10);
  }
  
  void flush() {
    ssport.flush();
  }

};




