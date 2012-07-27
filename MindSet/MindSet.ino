#include "MindSet.h"
#include <Monitor.h>

MindSet m3band(Serial1);

long prev;

void setup() {
  pinMode(13, OUTPUT);
  Serial1.begin(57600);
  Serial.begin(38400);
  Serial.println("Hi.");
  prev = millis();
  Serial1.write(0xc2);
}

void loop() {
  boolean result;
  result = m3band.getPacket();
  if ( result ) {
    digitalWrite(13, HIGH);
    m3band.parsePayload();
    digitalWrite(13, LOW);

    if ( millis() > prev + 500 && m3band.EEG_updated ) {
      prev = millis();
      //
      //Serial << "result: " << (result? "true" : "false" );
      //Serial << " code: ";
      //Serial.print(m3band.payload[m3band.pos_payhead], HEX);
      /*
      Serial << Monitor::endl;
      for (int i = 0; i < m3band.packetLength(); i++) {
        Serial.print(m3band.payload[i]>>4 & 0x0f, HEX);
        Serial.print(m3band.payload[i] & 0x0f, HEX);
        if ( i % 20 == 19 ) 
          Serial.println();
        else
          Serial.print(" ");
      }
      */
      Serial << Monitor::endl;
      if ( m3band.poorSignalQuality() > 0 ) {
        if ( m3band.poorSignalQuality() == 200 )
          Serial << "headset detached!";
        else
          Serial << "poor q: " << m3band.poorSignalQuality();
        Serial << Monitor::endl;
      }
      Serial << " att: " << m3band.attention;
      Serial << " med: " << m3band.meditation;
      Serial << " wave: " << m3band.rawWave 
        << Monitor::endl;
      if ( m3band.EEG_updated ) {
        Serial << "De: " << m3band.EEG[0];
        Serial << ", Th: " << m3band.EEG[1]; 
        Serial << ", L.A: " << m3band.EEG[2];
        Serial << ", H.A: " << m3band.EEG[3];
        Serial << ", L.B: " << m3band.EEG[4];
        Serial << ", H.B: " << m3band.EEG[5];
        Serial << ", L.G: " << m3band.EEG[6];
        Serial << ", M.G: " << m3band.EEG[7]
          << Monitor::endl;
      }
      Serial << Monitor::endl;

      //      Serial.print("length = ");
      //      Serial.println(packetLength);
      delay(233);
      //  } 
      //  }  
      /*
  else {
       Serial.print("result = ");
       Serial.println((result?"true":"false"));
       for (int i = 0; i < m3band.packetLength(); i++) {
       Serial.print(m3band.pBuffer[i]>>4 & 0x0f, HEX);
       Serial.print(m3band.pBuffer[i] & 0x0f, HEX);
       if ( i % 20 == 19 ) 
       Serial.println();
       else
       Serial.print(" ");
       }
       delay(733); */
    }
  }
}

















