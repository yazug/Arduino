/**
 *  @title:  StrongLink SL018/SL030 RFID reader demo
 *  @author: marc@marcboon.com
 *  @see:    http://www.stronglink.cn/english/sl018.htm
 *  @see:    http://www.stronglink.cn/english/sl030.htm
 *
 *  Arduino to SL018/SL030 wiring:
 *  A4/SDA     2     3
 *  A5/SCL     3     4
 *  5V         4     -
 *  GND        5     6
 *  3V3        -     1
 */
#include <Wire.h>
#include <StrongLinkI2C.h>

StrongLinkI2C reader;
StrongLinkI2C::CardInfo card;

void setup() {
  Wire.begin();
  Serial.begin(19200);

  // prompt for tag
  Serial.print("Reader firmware version ");
  Serial.println( reader.firmware_version() );
  Serial.println("Show me your tag");
}

void loop() {
  int i, n;
  char str[StrongLinkI2C::TYPENAME_MAXLENGTH];

  // start seek mode
  if ( reader.select() ) {
    if ( !reader.identical(card) ) {
      reader.get_card_info(card);
      for ( i = 0; i < card.uid_length; i++) {
        Serial.print(card.uid[i]>>4&0x0f, HEX);
        Serial.print(card.uid[i]&0x0f, HEX);
        Serial.print(" ");
      }
      if ( reader.card_type() ) {
        Serial.print(" type: ");
        reader.get_type_name(str);
        Serial.println(str);
      }
    }
  } 
  else {
    if ( card.uid_length != 0 ) {
      memset((void*) &card, 0, StrongLinkI2C::CARDINFO_LENGTH);
      Serial.println("Lost.");
      delay(500);
    }
  } 
  delay(50);
  //  }
}





