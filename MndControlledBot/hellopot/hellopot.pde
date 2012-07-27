// hellopot.pde - Print values from a potentiometer. 
// (c) Kimmo Karvinen & Tero Karvinen http://MindControl.BotBook.com

const int potPin=A0;        // <1>

void setup()
{
        pinMode(potPin, INPUT);      // <2>
        digitalWrite(potPin, HIGH);  // internal pullup // <3>
        
        Serial.begin(9600); // bit/s // <4>
}

void loop()
{
        int n=analogRead(potPin); // <5>
        Serial.println(n);        // <6>
        delay(400);               // <7>
}

