/*
  Web Server
 
 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 
 */

#include <Wire.h>
#include "PN532_I2C.h"

PN532 nfc(PN532::I2C_ADDRESS, 2, 3);
const byte cardkey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };

#include <SPI.h>
#include <Ethernet_SS.h>
#include "DS3234.h"
#include <string.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(131,206,18, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

DS3234 rtc(8);

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(19200);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Wire.begin();
  nfc.begin();
  unsigned long r = nfc.getFirmwareVersion();
  Serial.print("Version code: ");
  Serial.println(r, HEX);
  Serial.print("SAMConfiguration: ");
  Serial.println(nfc.SAMConfiguration(), HEX);


  SPI.begin();
  rtc.begin();
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  Serial.print("Date and Time: ");
  char tmp[32];
  rtc.update();
  Serial.print(rtc.calendarString(tmp));
  Serial.print(" ");
  Serial.print(rtc.timeString(tmp));
  Serial.println();
  
}


void loop() {
  char tmp[32];
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
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
                    // add a meta refresh tag, so the browser pulls again every 5 seconds:
          client.println("<meta http-equiv=\"refresh\" content=\"10\">");
          // output the value of each analog input pin
          rtc.update();
          client.print(rtc.calendarString(tmp));
          client.print(" ");
          client.print(rtc.timeString(tmp));
          client.println("<BR/>");
          //
          if ( (c = nfc.listPassiveTarget((byte*)tmp)) != 0 ) {
            client.println(" Mifare");
            client.print("Card UID bytes: ");
            for(int i = 0; i < tmp[5]; i++) {
              client.print( (byte)(tmp[6+i]), HEX);
            }
            client.println("<BR/>");
            if ( !nfc.mifare_AuthenticateBlock((uint8_t*)tmp+6, tmp[5], 4, 0, (byte*) cardkey_b+1) ) {
              client.println("Authenticated. <BR/>");
              if ( nfc.mifare_ReadDataBlock(4,(byte*)tmp) ) {
                client.println("Block 0 of sector 4: <BR/>");
                client.print("CLASS code: ");
                client.print((char)tmp[0]);
                client.print((char)tmp[1]);
                client.print(" ID code: ");
                for(int i = 2; i < 10; i++) {
                  client.print((char)tmp[i]);
                }
                client.print(" ISSUE code: ");
                  client.print((char)tmp[10]);
                client.println("<BR/>");
              }
            } else {
              client.println("Authentication failed.<br/>");
            }

          } else {
            client.println("No card present.<BR/>");
            client.println("<BR/>");
          }
          //
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");       
          }
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

