#include <SPI.h>
#include <Usb_HSv10.h>

#include <PaSoRi.h>
#include "ISO14443.h"

PaSoRi pasori;
const word syscode = POLLING_FCF;

template<class T>
inline Stream & operator<<(Stream & s, T arg) {
  s.print(arg);
  return s;
}
const char endl = '\n';

void setup() {
  SPI.begin();
  Serial.begin(57600);
  Serial << "Start" << endl;

  byte rcode = pasori.begin(); // initialize PaSoRi
  if (rcode != 0) {
    Serial.print("PaSoRi initialization failed! : rcode = ");
    Serial.println(rcode, HEX);
    while (1); // stop
  }
  Serial << "PaSoRi initialized." << endl;
}

void loop()
{
  byte rcode, i;
  byte type;
  pasori.task(); // call this at every loop

  //  rcode = pasori.poll(syscode);
  type = TypeA;
  rcode = pasori.listPassiveTarget(type, (byte*)"", 0);
  //  rcode = pasori.listPassiveTarget(TypeF, (byte*)"\x00\xFE\x00\x00\x00", 5);
  if (rcode) {
    delay(500);
  } 
  else {
    // Polling successful
    if ( type == TypeA ) {
      Serial << "Mifare " << "detected. UID = ";
      for (i = 1; i < pasori.idm[0]; i++) {
        Serial.print(pasori.idm[i]>>4, HEX);
        Serial.print(pasori.idm[i]&0x0f, HEX);
        Serial.print(" ");
      }
      Serial << "<< MagTape >>" << endl;
      readMifare();
      Serial << endl;
      
    } 
    else if ( type == TypeF ) {
      Serial << "FeliCa " << "detected. IDm = ";
      for (i = 0; i < 8; i++) {
        Serial.print(pasori.idm[i]>>4, HEX);
        Serial.print(pasori.pmm[i]&0x0f, HEX);
        Serial.print(" ");
      }
      Serial << "<< FCF >>" << endl;
      readFCF();
      Serial << endl;
    }

    delay(3000);
  }
}

int readMifare() {
  #define MIFARE_CMD_AUTH_A                   (0x60)
  byte tmp[32];
  tmp[0] = 0x60;
  tmp[1] = 2;
  memcpy(tmp+2, "\xFF\xFF\xFF\xFF\xFF\xFF",6);
  memcpy(tmp+8, pasori.idm+1,7);
  int rcode = pasori.sendInDataExchange(tmp,15);
}

int readFCF()
{
  byte buf[16*4];
  int ret = -1;
  for(int i = 0; i < 4; i++) {
    int rcode = pasori.read_without_encryption02(0x1a8b,i,buf+(i*16));
    if (rcode) {
      Serial.print("rcode = ");
      Serial.println(rcode, HEX);
    } 
    else {
      Serial << i << ": ";
      for(int p = 0; p < 16; p++) {
        Serial.print(buf[p+i*16] >> 4 & 0x0f, HEX);
        Serial.print(buf[p+i*16] & 0x0f, HEX);
        Serial << " ";
      }
      Serial << " ";
      for(int p = 0; p < 16; p++) {
        if ( isprint(buf[p+i*16]) ) {
          Serial.print((char) buf[p+i*16]);
        } 
        else {
          Serial.print('.');
        }
      }
      Serial << endl;
    }
  }
  Serial << "??: " << (char)((FCF *) & buf)->res0[0] << (char)((FCF *) & buf)->res0[1] << ", ";
  char tmp[16];
  memcpy(tmp, ((FCF *) & buf)->id, 8);
  tmp[8] = 0;
  Serial << "ID: " << tmp << " - " << ((FCF *) & buf)->issue << endl;
  for(int p = 0; buf[16+p] != 0; p++) {
    if ( buf[16+p] < 0x7e ) {
      Serial << (char) buf[16+p] << " ";
    } 
    else {
      Serial << kana[(buf[16+p] - (byte)0xa0)] << " ";
    }
  }
  Serial << endl;
  memcpy(tmp, ((FCF *) & buf)->goodthru, 8);
  tmp[8] = 0;
  long goodthru = atol((char*)tmp);
  Serial << "Goodthru: " << goodthru/10000 << "/" 
    << goodthru/100%100 << "/" << goodthru%100 << endl;
  Serial << endl;
  return ret;
}




