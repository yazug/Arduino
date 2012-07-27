// hellorgb.pde - Mix colors with RGB LED
// (c) Kimmo Karvinen & Tero Karvinen http://MindControl.BotBook.com

const int redPin=9;        // <1>
const int greenPin=10;
const int bluePin=11;

void setup()
{
        pinMode(redPin, OUTPUT); // <2>
        pinMode(greenPin, OUTPUT);
        pinMode(bluePin, OUTPUT);
}

void loop()
{
        setColor(255, 0, 0);      // <3>
        delay(1000); // ms        // <4>
        
        setColor(255, 255, 255);
        delay(1000);
}

void setColor(int red, int green, int blue)
{
        analogWrite(redPin, 255-red);        // <5>
        analogWrite(greenPin, 255-green);
        analogWrite(bluePin, 255-blue);
}


