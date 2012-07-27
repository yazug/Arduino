// avoidline.pde - turn when black line seen
// (c) Tero Karvinen & Kimmo Karvinen http://MindControl.BotBook.com

const int linePin=4;
const int servoLeftPin=3;
const int servoRightPin=2;

void setup()
{
        pinMode(linePin, INPUT);
        pinMode(servoLeftPin, OUTPUT);
        pinMode(servoRightPin, OUTPUT);
}

void loop()
{
        while (lineDetected()) turn();        // <1>
        forward();
}

bool lineDetected()
{
        return ! digitalRead(linePin);        // <2>
}

void forward()
{
        for (int i = 0; i < 10; i++) { 
                pulseServo(servoLeftPin, 1500+500);
                pulseServo(servoRightPin, 1500-500);
        }
}

void turn()        // <3>
{
        for (int i = 0; i < 30; i++) { // <4>
                pulseServo(servoLeftPin, 1500+500);
                pulseServo(servoRightPin, 1500+500); // <5>
        }
}

void pulseServo(int pin, int microseconds)
{
        digitalWrite(pin, HIGH);
        delayMicroseconds(microseconds);
        digitalWrite(pin, LOW);
        delay(5);
}

