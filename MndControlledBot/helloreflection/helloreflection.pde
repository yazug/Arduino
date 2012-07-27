// hellorefelection.pde - turn led on if line detected
// (c) Kimmo Karvinen & Tero Karvinen http://MindControl.BotBook.com

const int tinyLedPin=13;        // <1>
const int linePin=4;

void setup()
{
        pinMode(tinyLedPin, OUTPUT);
        pinMode(linePin, INPUT);
}

void loop()
{
        if (LOW == digitalRead(linePin)) {        // <2>
                digitalWrite(tinyLedPin, HIGH);
        } else {
                digitalWrite(tinyLedPin, LOW);
        }
        delay(20);        // <3>
}


