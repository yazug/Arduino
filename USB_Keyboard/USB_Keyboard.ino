	 
int buttonPin = 9;  // Set a button to any pin

void setup()
{
  pinMode(buttonPin, INPUT);  // Set the button as an input
  digitalWrite(buttonPin, HIGH);  // Pull the button high
}

void loop()
{
    delay(1000);  // delay so there aren't a kajillion z's
    Keyboard.print(millis());  // send a 'z' to the computer via Keyboard HID
    Keyboard.print(',');
    Keyboard.print(' ');
}
1000, 2011, 3021, 4031, 5041, 6051, 7062, 8072, 9082, 10093, 11106, 12118, 13129, 14141, 15153, 16164, 17178, 18191, 19204, 20216, 21228, 22240, 23251, 24263, 25276, 26289, 
27301, 28314, 29326, 30338, 31349, 32361, 33374, 34386, 35399, 36412, 37425, 38436, 39448, 40461, 41473, 42485, 
43498, 44511, 
