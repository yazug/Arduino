/*
ProtoSnap Pro Mini 
 SparkFun Electronics 2011
 
 This is the ProtoSnap Pro Mini Test Firmware that comes on the board when you get it.
 Bascially, it's meant to test the basic functions of the board, and
 it's not a bad piece of code to look over if you're just learning
 Arduino.
 
 if you press the button you'll hear the buzzer make a tone
 
 if you cover the light sensor, you get a Tri-color light sequence (red, green, blue blinks)
 
 then if you open the Serial terminal at 9600 you should see 
 "I am a protosnap, waiting to hear a space"
 ** note this is only done once at setup, so you may need to reset the board to see this message pop up.
 Type a space into terminal and it should respond: "I heard that, oh snap!"
 
 Have fun with your ProtoSnap Pro Mini!!
 
 */

const int button = 7;
const int buzzer = 2;
const int light = A0;
const int red = 3;
const int blue = 6;
const int green = 5;


void setup() {    
  Serial.begin(9600);

  pinMode(buzzer, OUTPUT);  
  pinMode(red, OUTPUT);  
  pinMode(green, OUTPUT);  
  pinMode(blue, OUTPUT);

  pinMode(button, INPUT);
  Serial.println("I am a protosnap, waiting to hear space bar:");
}

void loop() {

  if(Serial.available() > 0) {
    if(Serial.read() == 32)
      Serial.println("I HEARD THAT, OH SNAP!");
  }

  while(digitalRead(button) == LOW) {
    digitalWrite(buzzer, HIGH);
    delayMicroseconds(400);
    digitalWrite(buzzer, LOW);
    delayMicroseconds(400);
  }


  int temp = analogRead(light);
  if(temp > 100) {
    digitalWrite(red, HIGH);
    digitalWrite(green, HIGH);
    digitalWrite(blue, HIGH);
  } else {
    digitalWrite(red, LOW);
    digitalWrite(green, HIGH);
    digitalWrite(blue, HIGH);
    delay(500);
    digitalWrite(red, HIGH);
    digitalWrite(green, LOW);
    digitalWrite(blue, HIGH);
    delay(500);
    digitalWrite(red, HIGH);
    digitalWrite(green, HIGH);
    digitalWrite(blue, LOW);
    delay(500);
  }
}

