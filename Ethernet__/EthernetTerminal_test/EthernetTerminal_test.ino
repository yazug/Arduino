#include <SPI.h>
#include <EEPROM.h>
#include "Ethernet.h"

#include "Monitor.h"

Monitor mon(Serial);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetServer server(1234);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(19200);
  mon << "Ethernet terminal test. port1234. " << endl;
  //   while (!Serial) {
  //    ; // wait for serial port to connect. Needed for Leonardo only
  //  }

  SPI.begin();
  // start the Ethernet connection and the server:
  init_Ethernet();
  server.begin();

  mon << "server is at " << Ethernet.localIP() << endl;

}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  char buf[128];
  char c;
  
  if (client) {
    mon << "new client" << endl;
    Monitor cmon(client);
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        cmon.readToken(buf);
        //char c = client.read();
        for(int pos = 0; buf[pos]; pos++) 
          buf[pos] = toupper(buf[pos]);
        mon << buf << endl;
        
        if ( strcmp("E", buf) == 0 ) {
          cmon << "MAC ";
          cmon.printHex(mac, 6);
          cmon << endl;
        } else 
        if ( strcmp("T", buf) == 0 ) {
          mon << "T command." << endl;
          // send a standard http response header
          client.println("<html>");
          client.println("<meta http-equiv=\"refresh\" content=\"5\">");           // add a meta refresh tag, so the browser pulls again every 5 seconds:
          client.println(millis());
          client.println("<br />");
          client.println("</html>");
          break;
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

