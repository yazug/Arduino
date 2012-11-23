//
// This example shows one way of using an LED as a light sensor.
// You will need to wire up your components as such:
//
//           + digital2
//           |
//           <
//           > 100 ohm resistor
//           <
//           |
//           |
//         -----
//          / \  LED, maybe a 5mm, clear plastic is good
//         -----
//           |
//           |
//           + digital3
//
// What we are going to do is apply a positive voltage at digital2 and
// a low voltage at digital3. This is backwards for the LED, current will
// not flow and light will not come out, but we will charge up the 
// capacitance of the LED junction and the Arduino pin.
//
// Then we are going to disconnect the output drivers from digital2 and
// count how long it takes the stored charge to bleed off through the 
// the LED. The brighter the light, the faster it will bleed away to 
// digital3.
//
// Then just to be perverse we will display the brightness back on the 
// same LED by turning it on for a millisecond. This happens more often
// with brighter lighting, so the LED is dim in a dim room and brighter 
// in a bright room. Quite nice.
//
// (Though a nice idea, this implementation is flawed because the refresh
// rate gets too long in the dark and it flickers disturbingly.)
//
#define LED_N_SIDE 2
#define LED_P_SIDE 3

void setup()
{
  Serial.begin(9600); 
  pinMode(5,OUTPUT);
  analogWrite(5,128);
  pinMode(11,OUTPUT);
}

int sensor;
int sensor_charged;
unsigned int max_wait = 25600;
unsigned int min_wait = 500;
unsigned int last_timeout;
byte pwm_value = 0;

void chargeLED()
{
  // Apply reverse voltage, charge up the pin and led capacitance
  pinMode(LED_N_SIDE,OUTPUT);
  pinMode(LED_P_SIDE,OUTPUT);
  digitalWrite(LED_N_SIDE,HIGH);
  digitalWrite(LED_P_SIDE,LOW);

  while(sensor < 1010)
  {
    sensor = analogRead(A0);
  }
  // Isolate the pin 2 end of the diode
  pinMode(LED_N_SIDE,INPUT);
  digitalWrite(LED_N_SIDE,LOW);  // turn off internal pull-up resistor
}

  
void loop()
{
  unsigned int j;
  int last_tick;
  int now;

  chargeLED();


  sensor = sensor_charged = analogRead(A0);
  now = last_tick = millis();
  // Count how long it takes the diode to bleed back down to a logic zero
  while(now - last_tick < 10 && sensor > 100)
  {
    sensor = analogRead(A0);
    now = millis();
   // delayMicroseconds(1);
    
  }
  
  int discharge = (sensor_charged-sensor);
  int elapsed = now-last_tick;
  
  int rate = map((elapsed > 0 ? discharge/elapsed:discharge),0,1023,5,255);
  if(pwm_value < 256 && rate > pwm_value)
    pwm_value ++;
  else if( pwm_value > 0 && rate < pwm_value)
    pwm_value --;

  

  // Turn the light on for 1000 microseconds
  analogWrite(11,pwm_value);
  digitalWrite(LED_P_SIDE,HIGH);
  digitalWrite(LED_N_SIDE,LOW);
  pinMode(LED_P_SIDE,OUTPUT);
  pinMode(LED_N_SIDE,OUTPUT);
  delayMicroseconds(100);
  // we could turn it off, but we know that is about to happen at the loop() start

  Serial.print(discharge);
  Serial.print("\t");
  Serial.print(elapsed);
  Serial.print("\t");
  Serial.print(rate);
  Serial.print("\t");
  Serial.print(pwm_value);
  Serial.print("\n");
}

