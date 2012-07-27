// helloattention.pde - Show attention level (EEG) with LED color. 
// (c) Kimmo Karvinen & Tero Karvinen http://MindControl.BotBook.com

/* Disconnect TX and RX jump wires from Arduino when 
   uploading from IDE. 
   Turn robot on, then in a couple of seconds turn headband on. 
 */

const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;
const int tinyLedPin = 13;
const int speakerPin = 12;
const int servoRightPin = 2;
const int servoLeftPin = 3;

const int linePin = 4;
const int potPin = A0;

const float potMin = 14.0 - 1;
const float potMax = 236.0 - 5;

int tinyLedState = HIGH;

// percent variables, 0.0 to 1.0:
float tr = -1.0;
float attention = 0;
float speed = 0.0;

void setup()
{
        pinMode(redPin, OUTPUT);
        pinMode(greenPin, OUTPUT);
        pinMode(bluePin, OUTPUT);
        pinMode(tinyLedPin, OUTPUT);
        pinMode(speakerPin, OUTPUT);

        pinMode(servoRightPin, OUTPUT);
        pinMode(servoLeftPin, OUTPUT);

        pinMode(linePin, INPUT);

        pinMode(potPin, INPUT);
        digitalWrite(potPin, HIGH);        // internal pullup

        Serial.begin(115200);        // bit/s
        connectHeadset();
}

void loop()
{
        while (lineDetected()) turn();
        updateAttention();        // <1>
        tr = getThreshold();      // <2>
        if (attention > 0)
                setBlueToRed(attention);
        if (attention > tr) {
                speed = attention;        // <3>
        } else {
                speed = speed * 0.98;     // <4>
        }
        forward(speed);        // <5>
        toggleTinyLed();
}

float getThreshold()
{
        int x = analogRead(potPin);
        return (x - potMin) / potMax;
}

/*** Input: Other ***/

bool lineDetected()
{
        return !digitalRead(linePin);
}

/*** Input: Headset ***/

void connectHeadset()
{
        setGreen();
        delay(3000);
        Serial.write(0xc2);
        attention = 0;
        setWhite();
}

byte ReadOneByte()
{
        while (!Serial.available()) { }
        return Serial.read();
}

float updateAttention()
{
        byte generatedChecksum = 0;
        byte checksum = 0;
        int payloadLength = 0;
        byte payloadData[64] = { 0 };
        int poorQuality = 0;

        while ((170 != ReadOneByte()) && (0 < Serial.available())) {
                smartFlush();        // <6>

        }
        if (170 != ReadOneByte()) return -1;        // <7>

        /* Length */
        payloadLength = ReadOneByte();
        if (payloadLength > 169)
                return -2;

        /* Checksum */
        generatedChecksum = 0;
        for (int i = 0; i < payloadLength; i++) {
                payloadData[i] = ReadOneByte(); // Read payload into array
                generatedChecksum += payloadData[i];
        }
        generatedChecksum = 255 - generatedChecksum;
        checksum = ReadOneByte();
        if (checksum != generatedChecksum) {
                return -3;
        }

        /* Analyse payload */
        for (int i = 0; i < payloadLength; i++) {
                switch (payloadData[i]) {
                case 0xD0:
                        sayHeadsetConnected();
                        break;
                case 4:
                        i++;
                        attention = payloadData[i] / 100.0; // <8>
                        break;
                case 2:
                        i++;
                        poorQuality = payloadData[i];
                        if (170 < poorQuality) {        // max 200
                                setYellow();
                                attention = 0.0;
                                return -4;
                        }
                        break;
                case 0xD1:        // Headset Not Found
                case 0xD2:        // Headset Disconnected
                case 0xD3:        // Request Denied
                case -70:
                        wave(speakerPin, 900, 500);
                        attention = 0.0;
                        setWhite();
                        return -5;
                        break;
                case 0x80:
                        i = i + 3;
                        break;
                case 0x83:
                        i = i + 25;
                        break;
                } // switch
        } // for
}

void smartFlush()
{
        if (128 / 2 < Serial.available()) {   // buffer is 128 B // <9>
                Serial.flush();
        }
}

/*** Outputs: Servos ***/

void forward(float speed)
{
        if (speed <= 0)
                return;
        for (int i = 0; i < 2; i++) {
                pulseServo(servoLeftPin, 1500 + 500 * speed);
                pulseServo(servoRightPin, 1500 - 500 * speed);
        }
}

void turn()
{
        for (int i = 0; i < 20; i++) {
                pulseServo(servoLeftPin, 1500 + 500);
                pulseServo(servoRightPin, 1500 + 500);
        }
}

void pulseServo(int pin, int microseconds)
{
        digitalWrite(pin, HIGH);
        delayMicroseconds(microseconds);
        digitalWrite(pin, LOW);
        updateAttention();        // <10>
}

/*** Outputs: LED ***/

void setBlueToRed(float redPercent)
{
        int red = redPercent * 255;
        int blue = (1 - redPercent) * 255;
        setColor(red, 0, blue);
}

void setGreen()
{
        setColor(0, 255, 0);
}

void setYellow()
{
        setColor(255, 255, 0);
}

void setWhite()
{
        setColor(100, 100, 100);
}

void setColor(int red, int green, int blue)
{
        analogWrite(redPin, 255 - red);
        analogWrite(greenPin, 255 - green);
        analogWrite(bluePin, 255 - blue);
}

void toggleTinyLed()
{
        tinyLedState = !tinyLedState;
        digitalWrite(tinyLedPin, tinyLedState);
}

/*** Outputs: Speaker ***/

void sayHeadsetConnected()
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
        float period = 1 / frequency * 1000 * 1000; // microseconds
        long int startTime = millis();
        while (millis() - startTime < duration) {
                digitalWrite(pin, HIGH);
                delayMicroseconds(period / 2);
                digitalWrite(pin, LOW);
                delayMicroseconds(period / 2);
        }
}
