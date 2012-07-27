// allbutmind.pde - a mind controlled robot without mind control 
// (c) Tero Karvinen & Kimmo Karvinen http://MindControl.BotBook.com


const int linePin=4;
const int servoLeftPin=3;
const int servoRightPin=2;
const int potPin=A0;
const int redPin=9;
const int greenPin=10;
const int bluePin=11;
const float potMin=14.0-1;        // <1>
const float potMax=236.0-5;
const int speakerPin=12;

float speed=0.0;        // <2>

void setup()
{
        pinMode(linePin, INPUT);
        pinMode(servoLeftPin, OUTPUT);
        pinMode(servoRightPin, OUTPUT);
        pinMode(potPin, INPUT);
        digitalWrite(potPin, HIGH); // internal pullup
        pinMode(speakerPin, OUTPUT);
        
        sayReady();        // <3>
}

void loop()
{
        while (lineDetected()) turn();
        
        float at=getAttention();        // <4>
        float tr=getThreshold();        // <5>
        speed=fakeInertia(speed, at, tr);        // <6>
        
        forward(speed);
}

float fakeInertia(float speed, float at, float tr)
{
        if (at<tr)         // <7>
                speed-=0.05;
        else
                speed=at;        // <8>
        if (speed<0) speed=0.0;        // <9>
        return speed;
}

/*** Inputs ***/

bool lineDetected()
{
        return ! digitalRead(linePin);
}

float getAttention()
{
        return 0.5; // dummy value for testing        // <10>
}

float getThreshold()
{
        int x=analogRead(potPin);        // <11>
        float tr=(x-potMin)/potMax;      // <12>
        setBlueToRed(tr);
        return tr;
}

/*** Outputs ***/

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
        int red=redPercent*255;        // <13>
        int blue=(1-redPercent)*255;
        setColor(red, 0, blue);
}

void setColor(int red, int green, int blue)
{
        analogWrite(redPin, 255-red);
        analogWrite(greenPin, 255-green);
        analogWrite(bluePin, 255-blue);
}

void sayReady()
{
        wave(speakerPin, 440, 40);
        delay(25);
        wave(speakerPin, 300, 20);
        wave(speakerPin, 540, 40);
        delay(25);
        wave(speakerPin, 440, 20);
        wave(speakerPin, 640, 40);
        delay(25);
        wave(speakerPin, 540, 40);
        delay(25);
}

void wave(int pin, float frequency, int duration)
{
        float period = 1/frequency * 1000 * 1000; // microseconds
        long int startTime = millis();
        while (millis()-startTime < duration) {
                digitalWrite(pin, HIGH);
                delayMicroseconds(period / 2);
                digitalWrite(pin, LOW);
                delayMicroseconds(period / 2);
        }
}



