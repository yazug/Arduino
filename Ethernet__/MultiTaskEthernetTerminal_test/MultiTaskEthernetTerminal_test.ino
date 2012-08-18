#include <SPI.h>
#include <EEPROM.h>
#include "Ethernet.h"
#include "DS3234.h"
#include "Wire.h"
#include "PN532_I2C.h"
#include "ISO14443.h"

#include "Monitor.h"

PN532 nfc(PN532::I2C_ADDRESS, 2, 7);
byte polling[] = { 
  2, TypeF, TypeA };
enum {
  IDLE, 
  POLLING_REQUESTED,
} nfc_status = IDLE;
long lastpoll;

byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetServer server(1234);
Monitor mon(Serial);
EthernetClient client;

DS3234 rtc_spi(9);
long lastrtcupdate;

char buf[128];
char * bufp;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(19200);
  mon << "Ethernet terminal test @ port 1234. " << endl;
  //   while (!Serial) {
  //    ; // wait for serial port to connect. Needed for Leonardo only
  //  }

  SPI.begin();
  rtc_spi.begin();
  // start the Ethernet connection and the server:
  init_Ethernet();
  server.begin();
  mon << "server is at " << Ethernet.localIP() << endl;

  Wire.begin();
  nfc.begin();
  PN532_init();

  bufp = buf;
  *bufp = 0;
}


void loop() {
  // listen for incoming clients
  char c;
  byte cnt;
  long current;

  if ( millis() > lastrtcupdate + 133 ) {
    current = rtc_spi.time;
    rtc_spi.update();
    if ( current != rtc_spi.time ) {
      rtc_spi.printTimeOn(mon);
      mon << endl;
    }
  }

if ( ilde.
  if ( millis() > lastpoll + 233 ) {
    if ( nfc_status == IDLE ) {
    if ( nfc.InAutoPoll(1, 1, polling+1, polling[0]) ) 
      nfc_status = POLLING_REQUESTED;
      lastpoll = millis();
    } else {
      nfc_status = IDLE;
      mon << "AutoPoll Error. " << endl;
    }
    if ( nfc_status == POLLING_REQUESTED && nfc.IRQ_ready() ) {
      cnt = nfc.getCommandResponse((byte*) buf);
      if ( cnt ) {
        mon.printHex(buf, cnt);
        mon << endl;
      }
    }
  }

  if ( !client ) {
    if ( client = server.available() ) {
      mon << "new client opened." << endl;
    }
  } 
  else if ( client ) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    if (client.connected()) {
      if (client.available()) {
        Monitor cmon(client);
        if ( cmon.readLine(bufp) ) {
          if ( strcmp("MAC", buf) == 0 ) {
            cmon << ">> " << "MAC ";
            cmon.printHex(mac, 6);
            cmon << endl;
            mon << "MAC command. " << endl;
            //
            bufp = buf;
            *bufp = 0;
          } 
          else 
            if ( strcmp("TIME", buf) == 0 ) {
            cmon << ">> " << "TIME ";
            rtc_spi.printCalendarOn(cmon);
            cmon << " - ";
            rtc_spi.printTimeOn(cmon);
            cmon << endl;
            mon << "TIME command. " << endl;
            //
            bufp = buf;
            *bufp = 0;
          }
          else 
            if ( strcmp("QUIT", buf) == 0 ) {
            cmon << ">> " << "bye bye." << endl;
            delay(5);
            client.stop();
            mon << "disonnected client." << endl;
            //
            bufp = buf;
            *bufp = 0;
          } 
          else {
            cmon << ">>" << buf << endl;
            //
            bufp = buf;
            *bufp = 0;
          }
        }
      }
    }
    // close the connection:
  }
}

void init_Ethernet() {
  if ( EEPROM.read(0) == 0x9e && EEPROM.read(1) ) {
    for(int i = 0; i < 6; i++) {
      mac[i] = EEPROM.read(2+i);
    }
  }
  IPAddress ip(192,168,1, 177);
  IPAddress gateway(192,168,1, 1);
  IPAddress subnet(255, 255, 0, 0);
  Ethernet.begin(mac, ip, gateway, subnet);
}

boolean PN532_init() {
  byte cnt = 0;
  for (int i = 0; i < 3  && !( nfc.GetFirmwareVersion() && (cnt = nfc.getCommandResponse((byte*)buf)) ); i++) 
    delay(500);
  if (! cnt ) {
    Serial << "Couldn't find PN53x on Wire." << endl;
    return false;
  } 
  Serial << "PN53x IC ver. " << (char)buf[0] << ", Firmware ver. " 
    << buf[1] << '.' << buf[2] << endl;

  if ( nfc.SAMConfiguration() && nfc.getCommandResponse((byte*)buf) 
    && nfc.status() == PN532::RESP_RECEIVED) {
    Serial << "SAMConfiguration," << endl;
  }
  nfc.CPU_PowerMode(2);
  nfc.getCommandResponse((byte*)buf);
  nfc_status = IDLE;
  return true;
}






