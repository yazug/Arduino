
const int pin_shortp = 6;
const int pin_shortm = A2;
const int pin_longp = A1;
const int pin_longm = 7;

const int pin_longs = A2;
const int pin_shorts = A1;

void setup() {
  Serial.begin(9600);
  Serial.println("Hi.");
}

void loop() {
  int s, l;

  pinMode(pin_shortp, INPUT); 
  pinMode(pin_shortm, INPUT); 
  digitalWrite(pin_shortp, LOW);
  digitalWrite(pin_shortm, LOW);

  pinMode(pin_longp, OUTPUT);
  pinMode(pin_longm, OUTPUT);
  digitalWrite(pin_longp,HIGH);
  digitalWrite(pin_longm,LOW);

  l = analogRead(pin_longs);
  delayMicroseconds(776);
  l += analogRead(pin_longs);
  l /= 2;

  pinMode(pin_longp, INPUT);
  pinMode(pin_longm, INPUT);
  digitalWrite(pin_longp, LOW);
  digitalWrite(pin_longm, LOW);
  
  pinMode(pin_shortp, OUTPUT);
  pinMode(pin_shortm, OUTPUT);
  digitalWrite(pin_shortp, HIGH);
  digitalWrite(pin_shortm, LOW);

  s = analogRead(pin_shorts);
  delayMicroseconds(776);
  s += analogRead(pin_shorts);
  s /= 2;
  
  pinMode(pin_shortp, INPUT); 
  pinMode(pin_shortm, INPUT); 
  digitalWrite(pin_shortp, LOW);
  digitalWrite(pin_shortm, LOW);
  
  if ( l != 0 ) {
    Serial.print("l = ");
    Serial.print(l);
    Serial.print(", s = ");
    Serial.print(s);
    Serial.println();
    delay(100);
  }    
}

