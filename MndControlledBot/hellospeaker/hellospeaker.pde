// hellospeaker.pde - Beep a piezo speaker
// (c) Kimmo Karvinen & Tero Karvinen http://MindControl.BotBook.com

const int speakerPin=12;

void setup()
{
        pinMode(speakerPin, OUTPUT);
}

void loop()
{
        wave(speakerPin, 440, 500);        // <1>
        delay(50);        // <2>
        wave(speakerPin, 540, 500);
        delay(50);
        wave(speakerPin, 300, 400);
        delay(1000);
}

void wave(int pin, float frequency, int duration)
{
        float period = 1/frequency * 1000 * 1000; // microseconds // <3>
        long int startTime = millis();
        while (millis()-startTime < duration) {       // <4>
                digitalWrite(pin, HIGH);
                delayMicroseconds(period / 2);        // <5>
                digitalWrite(pin, LOW);
                delayMicroseconds(period / 2);
        }
}

