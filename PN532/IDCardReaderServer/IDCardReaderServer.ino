/*
  modified example in libraries Web Server
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 */
#include <EEPROM.h>

#include <SPI.h>
#include "Ethernet.h"
#include "DS3234.h"
#include "Wire.h"
#include "PN532_I2C.h"
#include "ISO14443.h"

#include "Monitor.h"

#include "IDCardReader.h"

DS3234 rtc(9);

#define IRQ   (2)
#define RESET (7)  // Not connected by default on the NFC Shield
PN532 nfc(PN532::I2C_ADDRESS, IRQ, RESET);
byte polling[] = { 
  2, TypeF, TypeA };
long prevPolling;
long prevDetect;

const byte IizukaKey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const byte factory_a[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

ISO14443 card;
byte buf[80];
struct {
  const static int table_size = 8;
  IDLog table[table_size];
  int index;
  int count;

  void init() {
    index = 0;
    count = 0;
    for(int i = 0; i < table_size; i++) {
      table[i].date = 0;
      table[i].time = 0;
      table[i].nfctype = 0;
    }
  }

  IDLog & last() {
    return table[index];
  }

  IDLog & at(const int i) {
    return table[(index + 1 + i) % count];
  }

  void add() {
    if ( count < table_size )
      count++;
    index++;
    if ( !(index < count ) )
      index %= count;
  }

} 
cardlog;

Monitor mon(Serial);
EthernetServer server(80);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(19200);
  //   while (!Serial) {
  //    ; // wait for serial port to connect. Needed for Leonardo only
  //  }

  SPI.begin();
  // start the Ethernet connection and the server:
  init_Ethernet();
  server.begin();
  mon << "server is at " << Ethernet.localIP() << endl;

  Wire.begin();
  nfc.begin();
  PN532_init();

  rtc.begin();
  rtc.update();

  card.init();
  Serial << "Start." << endl;
}


void loop() {
  byte cnt;
  ISO14443 tempcard;

  if ( millis() > prevPolling + 100 ) {
    //Serial.println("polling..");
    prevPolling = millis();
    if ( nfc.InAutoPoll(1, 1, polling+1, polling[0]) 
      && (cnt = nfc.getCommandResponse(buf)) ) {
      //  mon.printHex(buf, 8);
      // mon << endl;
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
          mon << "FeliCa FCF" << endl;
          if ( readFCF(card) ) {
            IDCardData & idcard = (IDCardData&) buf;
            cardlog.add();
            cardlog.last().time = rtc.time;
            cardlog.last().date = rtc.cal;
            cardlog.last().nfctype = card.type;
            memcpy(cardlog.last().nfcid, card.IDm, card.IDLength);
            memcpy(cardlog.last().id, idcard.felica.id, 8);
            memcpy(buf, idcard.felica.school, 8);
            buf[8] = 0;
            cardlog.last().school = strtol((char*)buf, NULL, HEX);
            cardlog.last().issue = idcard.felica.issue;
          }
          mon << "cardlog count = " << cardlog.count << endl;
        }
        else if ( card.type == 0x10 ) {
          rtc.update();
          prevDetect = millis();
          if ( readMifare(card) ) {
            IDCardData & idcard = (IDCardData&) buf;
            cardlog.add();
            cardlog.last().time = rtc.time;
            cardlog.last().date = rtc.cal;
            cardlog.last().nfctype = card.type;
            memcpy(cardlog.last().nfcid, card.UID, card.IDLength);
            memcpy(cardlog.last().id, idcard.mifare.id, 8);
            cardlog.last().school = 0;
            cardlog.last().issue = idcard.mifare.issue;
          }
          mon << "cardlog count = " << cardlog.count << endl;
        }
      } 
    } 
    else {
      mon << "Comm status: ";
      mon.print(nfc.communicationStatus(), HEX);
      mon << endl;
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
          client.println("<meta http-equiv=\"refresh\" content=\"10\">"); 
          // add a meta refresh tag, so the browser pulls again every 5 seconds:
          rtc.updateTime();
          char tmp[32];
          client.print(rtc.timeString(tmp));
          client.println("<br />");
          /*
          for(int cnum = 0; cnum < cardlog.count; cnum++) {
           client.print(cardlog.at(cnum).date, HEX);
           client.println("-");
           client.print(cardlog.at(cnum).time, HEX);
           client.println("-");
           client.print(cardlog.at(cnum).school, HEX);
           client.println("-");
           memcpy(tmp, cardlog.at(cnum).id, 8);
           tmp[8] = 0;
           client.print((char*) tmp);
           client.println("-");
           client.print((char)cardlog.at(cnum).issue);
           }
           */
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
  byte cnt = 0;
  for (int i = 0; i < 10 && !( nfc.GetFirmwareVersion() && (cnt = nfc.getCommandResponse(buf)) ); i++) 
    delay(150);
  if (! cnt ) {
    Serial << "Couldn't find PN53x on Wire." << endl;
    while (1); // halt
  } 
  Serial << "PN53x IC ver. " << (char)buf[0] << ", Firmware ver. " 
    << buf[1] << '.' << buf[2] << endl;

  nfc.SAMConfiguration();
  nfc.getCommandResponse(buf);
  if ( nfc.communicationStatus() == PN532::RESP_RECEIVED) {
    Serial << "SAMConfiguration," << endl;
    nfc.CPU_PowerMode(2);
  }
}

byte readFCF(ISO14443 & ccard) {
  int len;
  byte c;
  // Polling command, with system code request.
  len = nfc.felica_Polling(buf, 0x00fe);
  word scode = 0x1a8b; //, 0x170f, 0x1317, 0x1713, 0x090f, 0xffff 
  int snum = 0;
  word blklist[] = { 
    0,1,2,3               };
  //
  word codever = nfc.felica_RequestService(scode);
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
  byte res;
  mon << "Mifare ID ";
  mon.printHex(card.UID, card.IDLength);
  mon << endl;
  nfc.setUID(card.UID, card.IDLength, card.type);
  if ( nfc.mifare_AuthenticateBlock(4, IizukaKey_b) 
    && nfc.getCommandResponse(&res) && res == 0 ) {
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

void init_Ethernet() {
  byte mac[] = { 
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED       };
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








