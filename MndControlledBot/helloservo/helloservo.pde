// helloservo.pde - Spin continuous rotation servo
// (c) Kimmo Karvinen & Tero Karvinen http://MindControl.BotBook.com

void setup()
{
        pinMode(3, OUTPUT);        // <1>
}

void loop()
{
        digitalWrite(3, HIGH);
        delayMicroseconds(2000);   // <2>
        digitalWrite(3, LOW);
        delay(10); // ms           // <3>
}


