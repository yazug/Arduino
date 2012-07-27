#include "MindSet.h"
#include <Monitor.h>

MindSet m3band(Serial);

const int pin_r = 5;
const int pin_g = 3;
const int pin_b = 6;

struct RGBColor {
  int r, g, b;
  boolean blinking;

  RGBColor() {
    r = 0;
    g = 0;
    b = 0;
    blinking = false;
  }

  void set(int red, int gre, int blu) {
    r = red;
    g = gre;
    b = blu;
  }

  void blink() {
    blinking = true;
  }

  void noBlink() {
    blinking = false;
  }

  void lit(int rr, int gg, int bb) {
    analogWrite(pin_r, rr);
    analogWrite(pin_g, gg);
    analogWrite(pin_b, bb);
  }

  void lit() {
    if ( blinking && (millis() >> 9 & 1 == 0) ) {
      lit(0,0,0);

    } 
    else {
      lit(r,g,b);
    }
  }

};

RGBColor rgb;

long prev;

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(57600);
  //  Serial.begin(38400);
  //  Serial.println("Hi.");
  /*  
   Serial1.write("$$$");
   delay(100);
   while ( Serial1.available() ) {
   Serial.print((char)Serial1.read());
   }
   Serial1.print("R,1\r");
   delay(2000); 
   */
  prev = millis();
  Serial.write(0xc2);
}

void loop() {
  boolean result;
  result = m3band.getPacket();
  if ( result ) {
    digitalWrite(13, HIGH);
    m3band.parsePayload();
    digitalWrite(13, LOW);

    if ( /*millis() > prev + 100 &&*/ m3band.EEG_updated ) {
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
      //Serial << Monitor::endl;
      if ( m3band.poorSignalQuality() > 0 ) {
        rgb.blink();
        if ( m3band.poorSignalQuality() == 200 )
          rgb.set(200, 0, 0);
        //Serial << "headset detached!";
        else
          rgb.set(100, 100, 0);
        //Serial << "poor q: " << m3band.poorSignalQuality();
        //  Serial << Monitor::endl;
      } 
      else {
        rgb.noBlink();
        rgb.set(m3band.attention*2, 1.2*(m3band.attention+m3band.meditation), m3band.meditation*1.2);
      }
      //Serial << " att: " << m3band.attention;
      //Serial << " med: " << m3band.meditation;
      //Serial << " wave: " << m3band.rawWave 
      //  << Monitor::endl;
      if ( m3band.EEG_updated ) {
        /*
        Serial << "De: " << m3band.EEG[0];
         Serial << ", Th: " << m3band.EEG[1]; 
         Serial << ", L.A: " << m3band.EEG[2];
         Serial << ", H.A: " << m3band.EEG[3];
         Serial << ", L.B: " << m3band.EEG[4];
         Serial << ", H.B: " << m3band.EEG[5];
         Serial << ", L.G: " << m3band.EEG[6];
         Serial << ", M.G: " << m3band.EEG[7]
         << Monitor::endl;
         */
      }
      //      Serial << Monitor::endl;

      //      Serial.print("length = ");
      //      Serial.println(packetLength);
      //    delay(233);
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
  rgb.lit();
}











