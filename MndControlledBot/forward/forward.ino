// forward.pde - Drive the robot forward
// (c) Kimmo Karvinen & Tero Karvinen http://MindControl.BotBook.com

const int servoLeftPin=3;
const int servoRightPin=2;

void setup()
{
        pinMode(servoLeftPin, OUTPUT);
        pinMode(servoRightPin, OUTPUT);
}

void loop()
{
        forward();        // <1>
}

void forward()
{
        for (int i = 0; i < 10; i++) {        // <2>
                pulseServo(servoLeftPin, 1500+500);        // <3>
                pulseServo(servoRightPin, 1500-500);       // <4>
        }
}

void pulseServo(int pin, int microseconds)
{
        digitalWrite(pin, HIGH);
        delayMicroseconds(microseconds);
        digitalWrite(pin, LOW);
        delay(5); // ms        // <5>
}


