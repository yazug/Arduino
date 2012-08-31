#include <SPI.h>
#include "SD_SPI.h"
#include "Des.h"
#include "TextStream.h"
#include "SPISRAM.h"

#include "Wire.h"
#include "PN532_I2C.h"
#include "ISO14443.h"

#include "RecordBuffer.h"
// On the Ethernet Shield, CS is pin 4. 
const int chipSelect = 4;
SDClass SD(chipSelect);

const char * datafname = "keys.txt";
const char * dbfname = "keys.dbn";

const byte	uni_key[8]  = { 
  0x7F, 0x48, 0xE5, 0xEC, 0x12, 0x0F, 0xE1, 0x9E}; 	/* defoult net_key */
const byte	sys_key[8]  = { 
  0x5C, 0x78, 0x51, 0xCB, 0x6A, 0xB5, 0x4E, 0x5E};
//Des codec(uni_key);

TextStream mon(Serial);
byte buf[256];

#define IRQ   (2)
#define RESET (7)  // Not connected by default on the NFC Shield
PN532 nfc(PN532::I2C_ADDRESS, IRQ, RESET);

const byte IizukaKey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const byte factory_a[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

long prev;
ISO14443 card;

byte polling[] = {
  2,
  TypeA,
  TypeF
};

const int SRAM_CS = 10;
SPISRAM sram(SRAM_CS, SPISRAM::BUS_MBits); // CS pin

long cardcount;
RecordBuffer record;

void setup()
{
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  // Open serial communications and wait for port to open:
  Serial.begin(19200);
  while (!Serial);
  //delay(1000);

  Serial.print("Initializing SD card... ");

  SPI.begin();  //pinMode(10, OUTPUT);

  SD.begin() || (Serial.println("Card failed, or not present") && halt() );
  Serial.println("card initialized.");

  File datafile = SD.open(datafname);
  if ( !datafile ) Serial.println("Failed to open data file.");
  //  File dbfile = SD.open(dbfname, FILE_WRITE);
  //  if ( !dbfile ) Serial.println("Failed to open db file.");

  char buf[128];
  RecordBuffer record;

  long swatch = millis();
  if (datafile) {
    buf[0] = 0;
    TextStream rw(datafile);
    int istart, llen;
    char * p;
    cardcount = 0;
    boolean reol;
    while (rw.available()) {
      reol = false;
      // seems to wait for around 35 millisec for the first char ...
      if ( rw.concateLine(buf, 127, 50) || (datafile.size() == datafile.position()) ) {
        if ( buf[0] == '#' || (llen = strlen(buf)) == 0 ) {
          buf[0] = 0;
          reol = true;
          continue;
        }

        for(int i = 0; i < llen; i++) {
          if ( buf[i] == '\t' || buf[i] == ',' ) {
            buf[i] = 0;
          }
        }
        record.init();
        for(int itn = 0, istart = 0; istart < llen ; istart++) {
          if ( strlen(buf+istart) > 0 ) {
            switch(itn) {
            case 0:
              record.type = atoi(buf+istart);
              break;
            case 1:
              record.DIVISION = buf[istart];
              break;
            case 2:
              strncpy(record.PID, buf+istart, 8);
              break;
            case 3:
              record.ISSUE = buf[istart];
              break;
            case 4:
              TextStream::scanBytes(buf+istart, record.NFCID, 8);
              break;
            case 5:
              record.goodthru = strtol(buf+istart, NULL, 16);
              break;
            }
          }
          //Serial.print(itn);
          //Serial.print(buf+istart);
          //Serial.print(", ");
          itn++;
          istart += strlen(buf+istart);
        }
        //        Serial.print(cnt);
        //        Serial.print(" ");
        //        record.printOn(Serial);
        //        Serial.println();
        //        if ( !dbfile.write(record.rawbytes, 32) )
        //          Serial.println("write failed!!!");
        sram.write(cardcount * 24, record.rawbytes, 24);
        cardcount++;
        buf[0] = 0;
      }
    }
    datafile.close();
    //    dbfile.close();
    Serial.println(millis() - swatch);
    Serial.println("Loading data finished.");
    Serial.println();
    Serial.println("Now reading data from Serial SRAM.");
    swatch = millis();
    for(long i = 0; i < cardcount; i++) {
      sram.read(i*24, record.rawbytes, 24);
      if ( i < 4 || i > cardcount - 4 ) {
        Serial.print(i);
        Serial.print(" ");
        record.printOn(Serial);
//        if ( i == 4 ) 
//          Serial.println("...");
      }
    }
    Serial.println(millis() - swatch);
    Serial.println("Dumping ");
    Serial.print(cardcount);
    Serial.println(" x 24 bytes data finished.");
    Serial.println();
/*
    swatch = millis();

    Serial.println("Search by type, division, pid and issue no.");
    int pos = bsearch(cardcount, "\x10S825418541");
    Serial.print(millis() - swatch);
    Serial.println(" msec.");
    Serial.println(pos);
    sram.read(pos * 24, record.rawbytes, 24);
    record.printOn(Serial);
*/
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.print("error opening ");
    Serial.println(datafname);
  } 


  //
  //
  //
  Wire.begin();
  nfc.begin();
  //  lcd.begin();

  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);

  PN532_init();
  card.clear();

  delay(500);

}

void loop()
{
  ISO14443 tcard;
  byte c;

  if ( millis() > prev + 50 ) {
    prev = millis();
    if ( (c = nfc.InAutoPoll(1, 1, polling+1, polling[0])) && (c = nfc.getAutoPollResponse((byte*) buf)) ) {
      //mon << mon.printArray(tmp, 8) << mon.endl;
      // NbTg, type1, length1, [Tg, ...]
      tcard.set(buf[1], buf+3);
      if ( tcard != card ) { 
        card = tcard;
        if ( card.type == 0x11 ) {
          mon << "FeliCa IDm: ";
          mon.printBytes( card.IDm, 8);
          //          mon << ", PMm: ";
          //          mon.printBytes( card.PMm, 8);
          mon  << " detected. " << endl;

          if ( read_FCF() == 0 ) {
            mon << endl;
            tone(4, 1200, 100);
            card.clear();
          }
        } 
        else if ( card.type == 0x10 ) {
          char idstr[12];
          mon << "Mifare" << "  ID: ";
          mon.printBytes(card.UID, card.IDLength);
          mon << endl;
          nfc.targetSet(0x10, card.UID, card.IDLength);
          if ( nfc.mifare_AuthenticateBlock(4, IizukaKey_b) ) {
            mon << "Auth Success." << endl;
            nfc.mifare_ReadDataBlock(4, buf);
            memcpy(idstr, buf,11);
            idstr[0] = 0x10;
            mon.printBytes(buf, 16); 
            mon << "  ";
            mon.printBytes((char*) buf, 16); 
            mon << endl;
            nfc.mifare_ReadDataBlock(5, buf);
            mon.printBytes(buf, 16); 
            mon << "  ";
            mon.printBytes((char*)buf, 16); 
            mon << endl;
            nfc.mifare_ReadDataBlock(6, buf);
            mon.printBytes(buf, 16); 
            mon << "  ";
            mon.printBytes((char*)buf, 16); 
            mon << endl;
            //

            Serial.println("Search by type, division, pid and issue no.");
            int pos = bsearch(cardcount, idstr);

            Serial.println(" msec.");
            Serial.println(pos);
            sram.read(pos * 24, record.rawbytes, 24);
            record.printOn(Serial);

          } 
          else {
            tone(4, 1200, 100);
            Serial << "Failure." << endl;
          }
          Serial << endl;
        }
      } 
    }
    else {
      card.clear();
    }
  }
  //delay(250);
}

boolean halt() {
  for(;;);
  return true;
}

int bsearch(long limit, char id[]) {
  RecordBuffer r;
  /*
  for(int i = 0; i < 11; i++) {
   Serial.print(id[i], HEX);
   Serial.print(' ');
   }
   Serial.println();
   Serial.println();
*/   
  long ib = 0;
  long ie = limit;
  long ix;
  while (ie > ib) {
    ix = (ib+ie)/2;
    sram.read(ix * 24, r.rawbytes, 24);
    int diff = memcmp(r.rawbytes, id, 11);
    
    r.printOn(Serial);
     Serial.print(ix);
     Serial.print(": ");
     for(int i = 0; i < 11; i++) {
     Serial.print(r.rawbytes[i], HEX);
     Serial.print(' ');
     }
     Serial.print(" :");
     Serial.print(diff);
     Serial.println();
     Serial.println();
     
    if ( diff == 0 ) {
      return ix;
    }
    if ( diff > 0 ) {
      ie = ix;
    } 
    else {
      ib = ix + 1;
    }
  }
  return -1;
}


byte read_FCF() {
  word syscode = 0x00FE;
  int len;
  byte c;

  // Polling command, with system code request.
  len = nfc.felica_Polling(buf, syscode);
  if ( len == 0 )
    return 0;
  mon << "System code ";
  mon.printBytes((byte*)&syscode, 1);
  mon <<" specific IDm ";
  mon.printBytes(buf+1, 8);
  Serial << endl;
  // low-byte first service code.
  // Suica, Nimoca, etc. 0x090f system 0x0300
  // Edy service 0x170f (0x1317), system 0x00FE // 8280
  // FCF 1a8b
  mon << "Request Service code: ";
  word servcode = 0x1a8b;
  word scver = nfc.felica_RequestService(servcode);
  mon.print(servcode, HEX);
  mon << ": "; 
  mon.println(scver, HEX);

  if ( scver == 0xffff ) 
    return 0;
  word blist[] = { 
    0, 1, 2, 3  };
  c = nfc.felica_ReadBlocksWithoutEncryption(buf, servcode, (byte) 1, blist);
  if ( c == 0 ) {
    Serial << endl << "failed. " << endl;
    return 0;
  }
  mon.printBytes(buf, 16);
  mon << "  ";
  mon.printBytes((char*) buf, 16);
  mon << endl;
  mon << "--- End of FCF reading ---" << endl << endl;
  return 1;
}



void PN532_init() {
  byte cnt = 0;
  for (int i = 0; i < 3  && !( nfc.GetFirmwareVersion() && (cnt = nfc.getCommandResponse((byte*)buf)) ); i++) 
    delay(500);
  if (! cnt ) {
    Serial << "Couldn't find PN53x on Wire." << endl;
    return;
  } 
  Serial << "PN53x IC ver. " << (char) buf[0] << ", Firmware ver. " 
    << (byte) buf[1] << '.' << (byte) buf[2] << endl;

  if ( nfc.SAMConfiguration() && nfc.getCommandResponse((byte*)buf) 
    && nfc.status() == PN532::RESP_RECEIVED) {
    Serial << "SAMConfiguration," << endl;
  }
  nfc.CPU_PowerMode(2);
  nfc.getCommandResponse((byte*)buf);
  return;
}




