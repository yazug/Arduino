
const int pin_signal = 13;
void setup() {
  pinMode(pin_signal, OUTPUT);
  digitalWrite(pin_signal, LOW);
  
  Serial.begin(19200);
}


void loop() {
  int val0, val1;
  long swatch0, swatch1;
  digitalWrite(pin_signal, HIGH);
  delay(2000);
  val0 = analogRead(0);
  Serial.print(val0);
  Serial.print(" -> ");
  val0 = analogRead(0);
  swatch0 = millis();
  digitalWrite(pin_signal, LOW);
  for(int i = 0; i < 2000; i++) {
    delay(5);
    val1 = analogRead(0);
    swatch1 = millis();
    if ( val1 < val0/2 ) 
      break;
  }
  Serial.print(val1);
  Serial.print(" : ");
  Serial.println(swatch1 - swatch0);
  delay(2000);
}

