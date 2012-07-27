// threshold.pde - potentiometer selects minimum attention to move
// (c) Tero Karvinen & Kimmo Karvinen http://MindControl.BotBook.com


const int linePin=4;
const int servoLeftPin=3;
const int servoRightPin=2;
const int potPin=A0;
const int redPin=9;
const int greenPin=10;
const int bluePin=11;
const float potMin=14.0-1;
const float potMax=236.0-5;

void setup()
{
        pinMode(linePin, INPUT);
        pinMode(servoLeftPin, OUTPUT);
        pinMode(servoRightPin, OUTPUT);
        pinMode(potPin, INPUT);
        digitalWrite(potPin, HIGH); // internal pullup
}

void loop()
{
        while (lineDetected()) turn();
        float at=getAttention();
        float tr=getThreshold();
        setBlueToRed(tr);
        if (at>tr) forward(at);
}

bool lineDetected()
{
        return ! digitalRead(linePin);
}

float getAttention()
{
        return 0.5; // dummy value for testing
}

float getThreshold()
{
        int x=analogRead(potPin);
        return (x-potMin)/potMax;        // 
}

void forward(float speed)
{
        for (int i = 0; i < 10; i++) { 
                pulseServo(servoLeftPin, 1500+500*speed);
                pulseServo(servoRightPin, 1500-500*speed);
        }
}

void turn()
{
        for (int i = 0; i < 30; i++) {
                pulseServo(servoLeftPin, 1500+500);
                pulseServo(servoRightPin, 1500+500);
        }
}

void pulseServo(int pin, int microseconds)
{
        digitalWrite(pin, HIGH);
        delayMicroseconds(microseconds);
        digitalWrite(pin, LOW);
        delay(5);
}

void setBlueToRed(float redPercent)
{
        int red=redPercent*255;
        int blue=(1-redPercent)*255;
        setColor(red, 0, blue);
}

void setColor(int red, int green, int blue)
{
        analogWrite(redPin, 255-red);
        analogWrite(greenPin, 255-green);
        analogWrite(bluePin, 255-blue);
}

