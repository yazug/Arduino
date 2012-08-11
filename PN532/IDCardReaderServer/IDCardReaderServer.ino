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

#include "IDCardReader.h"

DS3234 rtc(8);

#define IRQ   (2)
#define RESET (0xff)  // Not connected by default on the NFC Shield
PN532 nfc(PN532::I2C_ADDRESS, IRQ, RESET);

const byte IizukaKey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const byte factory_a[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

ISO14443 card;
byte buf[80];
IDLog cardlog[8];

Monitor mon(Serial);

byte polling[] = {
  2,
  TypeF,
  TypeA
};
long prevPolling;
long prevDetect;


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
  Serial.begin(19200);
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

  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  card.init();
}


void loop() {
  ISO14443 tempcard;

  if ( millis() > prevPolling + 100 ) {
    prevPolling = millis();
    if ( nfc.autoPoll(polling, buf) ) {
      //mon << mon.printArray(tmp, 8) << mon.endl;
      // NbTg, type1, length1, [Tg, ...]
      tempcard.set(buf[1], buf+3);
      if ( tempcard == card 
        && millis() > prevDetect + 5000 ) {
        card.init();
      }
      if ( tempcard != card ) { 
        card.set(tempcard);
        if ( card.type == 0x11 ) {
          rtc.update();
          prevDetect = millis();
          if ( readFCF(card) ) {
            IDCardData & idcard = (IDCardData&) buf;
            cardlog[0].time = rtc.time;
            cardlog[0].date = rtc.cal;
            cardlog[0].nfctype = card.type;
            memcpy(cardlog[0].nfcid, card.IDm, card.IDLength);
            memcpy(cardlog[0].id, idcard.felica.id, 8);
            memcpy(cardlog[0].school, idcard.felica.school, 8);
            cardlog[0].issue = idcard.felica.issue;
          }

        }
        else if ( card.type == 0x10 ) {
          rtc.update();
          prevDetect = millis();
          if ( readMifare(card) ) {
            IDCardData & idcard = (IDCardData&) buf;
            cardlog[0].time = rtc.time;
            cardlog[0].date = rtc.cal;
            cardlog[0].nfctype = card.type;
            memcpy(cardlog[0].nfcid, card.UID, card.IDLength);
            memcpy(cardlog[0].id, idcard.mifare.id, 8);
            memset(cardlog[0].school, 0, 8);
            cardlog[0].issue = idcard.mifare.issue;
          }
        }
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
          rtc.updateTime();
          char tmp[32];
          client.print(rtc.timeString(tmp));
          client.println("<br />");
          client.print(cardlog[0].date, HEX);
          client.println("-");
          client.print(cardlog[0].time, HEX);
          client.println("-");
          for(int i = 0; i < 8; i++) {
            client.print((char) cardlog[0].school[i]);
          }
          client.println("-");
          for(int i = 0; i < 8; i++) {
            client.print((char) cardlog[0].id[i]);
          }
          client.println("-");
          client.print((char)cardlog[0].issue);
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
  // Got ok data, print it out!
  Serial << "Found chip PN5";
  Serial.println(r & 0xff, HEX);
  Serial << "Firmware ver. " << (r>>8 & 0xFF) << '.' 
    << (r>>16 & 0xFF) << endl;

  Serial << "SAMConfiguration" << endl;

  nfc.SAMConfiguration();
  Serial << endl;
}

byte readFCF(ISO14443 & ccard) {
  int len;
  byte c;
  // Polling command, with system code request.
  len = nfc.felica_Polling(buf, 0x00fe);
  //  mon << "Polling ";
  //  mon.print((word)0x00fe, HEX); 
  //  mon << " " ;
  word scode = 0x1a8b; //, 0x170f, 0x1317, 0x1713, 0x090f, 0xffff 
  int snum = 0;
  word blklist[] = { 
    0,1,2,3       };
  //
 word codever = nfc.felica_RequestService(scode);
  //  mon << "Service code = ";
  //  mon.print(scode, HEX); 
  //  mon << ", version = ";
  //  mon.print(codever, HEX); 
  //  mon << endl;
  if ( codever != 0xffff && scode != 0xffff) {
    c = nfc.felica_ReadBlocksWithoutEncryption(buf, scode, (byte) 4, blklist);
    if ( c != 0 ) {
      IDCardData & idcard((IDCardData &) buf);
      mon << "DIV. ";
      mon.printHex((char*) &idcard.felica.division,2);
      mon << "  SCHL "; 
      mon.printHex(idcard.felica.school, 8);
      mon << " ID "; 
      mon.printHex(idcard.felica.id, 8);
      mon << " - " << idcard.felica.issue 
        << " GEND. " << idcard.felica.gender << endl;
      mon << "ISSUE "; 
      mon.printHex(idcard.felica.dateofissue, 8);
      mon << " THRU "; 
      mon.printHex(idcard.felica.goodthru, 8);
      mon << endl;
      //
      mon << endl;
      return 1;
    }
  }
  return 0;
}

byte readMifare(ISO14443 & card) {
  mon << "Mifare ID ";
  mon.printHex(card.UID, card.IDLength);
  mon << endl;
  nfc.setUID(card.UID, card.IDLength, card.type);
  if ( nfc.mifare_AuthenticateBlock(4, IizukaKey_b) ) {
    for (int blk = 0; blk < 3; blk++) {
      if ( !nfc.mifare_ReadDataBlock(4+blk, buf+(blk*16)) )
        return 0;
    }
    IDCardData & idcard((IDCardData &) buf);
    mon << "DIV. ";
    mon.printHex((char*) &idcard.mifare.division, 2);
    //      mon << " SCHL "; mon.printHex(idcard.felica.school, 8);
    mon << "  ID "; 
    mon.printHex(idcard.mifare.id, 8);
    mon << " - " << idcard.mifare.issue 
      << " GEND. " << idcard.mifare.gender << endl;
    mon << "ISSUE "; 
    mon.printHex(idcard.mifare.dateofissue, 7);
    mon << endl;
    //
    mon << endl;
    return 1;
  } 
  return 0;
}




