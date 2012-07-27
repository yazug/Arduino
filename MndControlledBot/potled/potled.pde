// hellopot.pde - Control LED brightness with a potentiometer
// (c) Kimmo Karvinen & Tero Karvinen http://MindControl.BotBook.com

const int potPin=A0;
const float potMin=14.0-1;        // <1>
const float potMax=236.0-5; // <2>
const int redPin=9;
const int greenPin=10;
const int bluePin=11;
const int speakerPin=12;
const float f5=698.456; // Hz
const float a4=440;
const float c4=261.626;

void setup()
{
        pinMode(potPin, INPUT);
        digitalWrite(potPin, HIGH); // internal pullup
        
        pinMode(redPin, OUTPUT);
        pinMode(greenPin, OUTPUT);
        pinMode(bluePin, OUTPUT);
        
        pinMode(speakerPin, OUTPUT);
        wave(speakerPin, 440, 1000);
}

void loop()
{
        int x=analogRead(potPin);
        float p=(x-potMin)/potMax;        // <3>
        setBlueToRed(p);        // <4>
        if (p<0.05) wave(speakerPin, c4, 90);
        if (0.47<p && p<0.53) wave(speakerPin, a4, 90);
        if (0.95<p) wave(speakerPin, f5, 90);
        delay(10);
}

void setBlueToRed(float redPercent)
{
        int red=redPercent*255;        // <5>
        int blue=(1-redPercent)*255;        // <6>
        setColor(red, 0, blue);
}

void setColor(int red, int green, int blue)
{
        analogWrite(redPin, 255-red);
        analogWrite(greenPin, 255-green);
        analogWrite(bluePin, 255-blue);
}

void wave(int pin, float frequency, int duration)
{
        float period = 1/frequency * 1000 * 1000; // microseconds // <3>
        long int startTime = millis();
        while (millis()-startTime < duration) {        // <4>
                digitalWrite(pin, HIGH);
                delayMicroseconds(period / 2);        // <5>
                digitalWrite(pin, LOW);
                delayMicroseconds(period / 2);
        }
}

