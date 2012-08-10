/*
  modified example in libraries Web Server
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 */

#include <SPI.h>
#include "Ethernet.h"
#include "DS3234.h"
#include "Wire.h"
#include "PN532_I2C.h"
#include "ISO14443.h"

#include "Monitor.h"

DS3234 rtc(8);

#define IRQ   (2)
#define RESET (0xff)  // Not connected by default on the NFC Shield
PN532 nfc(PN532::I2C_ADDRESS, IRQ, RESET);

const byte IizukaKey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const byte factory_a[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

ISO14443 card;
byte buf[48];

byte polling[] = {
  2,
  TypeA,
  TypeF
};
long prevPolling;


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1, 177);
IPAddress gateway(192,168,1, 1);
IPAddress subnet(255, 255, 0, 0);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  //   while (!Serial) {
  //    ; // wait for serial port to connect. Needed for Leonardo only
  //  }

  SPI.begin();
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();

  rtc.begin();
  rtc.update();

  Wire.begin();
  nfc.begin();
  PN532_init();
  card.init();

  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  ISO14443 tempcard;

  if ( millis() > prevPolling + 100 ) {
    prevPolling = millis();
    if ( nfc.autoPoll(polling, buf) ) {
      //mon << mon.printArray(tmp, 8) << mon.endl;
      // NbTg, type1, length1, [Tg, ...]
      tempcard.set(buf[1], buf+3);
      if ( tempcard != card ) { 
        card = tempcard;
        if ( card.type == 0x11 ) {
          rtc.update();
        }
        else if ( card.type == 0x10 ) {
          rtc.update();
        }
      } 
      else {
        card.init();
      }
    }
  }

  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          send_header(client);
          client.println("<html>");
          client.println("<meta http-equiv=\"refresh\" content=\"10\">");           // add a meta refresh tag, so the browser pulls again every 5 seconds:
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");       
          }
          rtc.updateTime();
          char tmp[32];
          client.print(rtc.timeString(tmp));
          client.println("<br />");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

void send_header(EthernetClient & client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connnection: close");
  client.println();
  client.println("<!DOCTYPE HTML>");
}

void PN532_init() {
  char tmp[40];
  Monitor mon(tmp);

  Serial << "Firmware version: ";
  unsigned long r = 0;
  for (int i = 0; i < 10 ; i++) {
    if ( (r = nfc.getFirmwareVersion()) )
      break;
    delay(250);
  }
  if (! r ) {
    Serial << "Couldn't find PN53x on Wire." << endl;
    while (1); // halt
  } 
  else {
    mon.printHex((byte*)&r, 4); 
    Serial << endl;
  }

  // Got ok data, print it out!
  Serial << "Found chip PN5";
  Serial.println(r & 0xff, HEX);
  Serial << "Firmware ver. " << (r>>8 & 0xFF) << '.' 
    << (r>>16 & 0xFF) << endl;

  Serial << "SAMConfiguration" << endl;

  nfc.SAMConfiguration();
  Serial << endl;
}



