/*
 Chat  Server
 
 A simple server that distributes any incoming messages to all
 connected clients.  To use telnet to  your device's IP address and type.
 You can see the client's input in the serial monitor as well.
 Using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 
 */
#include <EEPROM.h>
#include <SPI.h>
#include <Ethernet_w5100.h>
#include "SD_SPI.h"
#include "Monitor.h"
#include "Des.h"

Des codec;
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1, 177);
IPAddress gateway(192,168,1, 1);
IPAddress subnet(255, 255, 255, 0);


// telnet defaults to port 23
EthernetServer authServer(5963);
EthernetServer commandServer(7602);
boolean authConnected = false; // whether or not the client was connected previously
boolean commConnected = false;

//SDClass SD(4);

void setup() {
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  SPI.begin();
  //  SD.begin();
  // initialize the ethernet device
  Ethernet.begin(mac, ip, gateway, subnet);
  // start listening for clients

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.print("Terminal address:");
  Serial.println(Ethernet.localIP());
  authServer.begin();

  codec.key_set((byte*)"\x12\x12\x12\x12\x12\x12\x12\x12");

}

void loop() {
  // wait for a new client:
  EthernetClient client = (!commConnected ? authServer.available() : client = commandServer.available() );  
  Monitor climon(client);
  char buf[128] = {
    0  };

  // when the client sends the first byte, say hello:
  if (client) {
    if (!authConnected) {
      // clear out the input buffer:
      client.flush();
      Serial.println("We have a new client");
      client.println("Hello, client!"); 
      authConnected = true;
    } 
    else 
      if ( client.connected() && client.available() > 0) {
      // read the bytes incoming from the client:
      if ( climon.concatenateLine(buf, 127) ) {
        authServer.println(buf);
        // echo the bytes to the server as well:
        codec.ecb_encrypt(buf, 8);
        climon.printBytes(buf, 8);
        climon << endl;
//        Serial.println(buf);
        if ( strcmp(buf, "quit.") == 0 ) {
          client.stop();
          authConnected = false;
        }
        buf[0] = 0;
      }
    }
  }
}









