/*
  * 3pi-wall-follower - demo code for the Pololu 3pi Robot
 *
 * If two Sharp distance sensors ( http://www.pololu.com/catalog/product/136 )
 * are installed, this code will allow a 3pi to explore its environment by
 * following objects it finds on its left side.
 *
 * http://www.pololu.com
 * http://forum.pololu.com
 *
 */

// The 3pi include file must be at the beginning of any program that
// uses the Pololu AVR library and 3pi.
#include <Pololu3pi.h>
#include <PololuQTRSensors.h>
#include <OrangutanMotors.h>
#include <OrangutanPushbuttons.h>
#include <OrangutanLCD.h>
#include <OrangutanBuzzer.h>
#include <OrangutanAnalog.h>

// This include file allows data to be stored in program space.  The
// ATmega168 has 16k of program space compared to 1k of RAM, so large
// pieces of static data should be stored in program space.
#include <avr/pgmspace.h>

// Introductory messages.  The "PROGMEM" identifier causes the data to
// go into program space.
const char welcome_line1[] PROGMEM = " Pololu";
const char welcome_line2[] PROGMEM = "3\xf7 Robot";
const char name_line1[] PROGMEM = "Wall";
const char name_line2[] PROGMEM = "Follower";

// A couple of simple tunes, stored in program space.
const char welcome[] PROGMEM = ">g32>>c32";
const char go[]      PROGMEM = "L16 cdegreg4";

// Refresh the LCD display every tenth of a second.
const int display_interval_ms = 100;
#define MS_ELAPSED_IS(n) (millis() % n == 0)
#define TIME_TO_DISPLAY (MS_ELAPSED_IS(display_interval_ms))

void do_welcome() {
  print_from_program_space(welcome_line1);
  lcd_goto_xy(0,1);
  print_from_program_space(welcome_line2);
  play_from_program_space(welcome);
  delay(1000);
  clear();
  print_from_program_space(name_line1);
  lcd_goto_xy(0,1);
  print_from_program_space(name_line2);
}

void display_battpower() {
  clear();
  print_long(read_battery_millivolts());
  print("mV");
  lcd_goto_xy(0,1);
  print("Press B");
  delay(100);
}

int last_proximity = 0;
const int base_speed = 200;
const int set_point = 100;

void setup() {
  // set up the 3pi
  // Set PC5 to read mode
  DDRC  &= ~(1<< 5); //PC5);
  PORTC &= ~(1<< 5); //PC5);

  // Play welcome music and display a message
  do_welcome();

  delay(1000);
  // Display battery voltage and wait for button press
  while(!button_is_pressed(BUTTON_B)) {
    display_battpower();
  }
  // Always wait for the button to be released so that 3pi doesn't
  // start moving until your hand is away from it.
  wait_for_button_release(BUTTON_B);
  clear();
  print("Go!");
  // Play music and wait for it to finish before we start driving.
  play_from_program_space(go);
  while(is_playing());

}

void loop() {
  // This is the "main loop" - it will run forever.

  // In case it gets stuck: for 1 second every 15 seconds back up
  if (millis() % 15000 > 14000) {
    back_up();
    return;
  }
  // If something is directly in front turn to the right in place
  int front_proximity = max(0, 670 - analog_read(5));
  if (front_proximity > 200) {
    turn_in_place();
    return;
  }
  int proximity = analog_read(7); // 0 (far away) - 650 (close)
  int proportional = proximity - set_point;
  int derivative = proximity - last_proximity;
  // Proportional-Derivative Control Signal
  // Proportional-Derivative Control Signal
  int pd = proportional / 3 + derivative * 20;
  int left_set  = base_speed - pd;
  int right_set = base_speed + pd;
  set_motors(left_set, right_set);
  if (TIME_TO_DISPLAY) {
    clear();
    lcd_goto_xy(0,0);
    print_long(proximity);
    lcd_goto_xy(5,0);
    print_long(pd);
    lcd_goto_xy(0,1);
    print_long(left_set);
    lcd_goto_xy(4,1);
    print_long(right_set);
  }
  last_proximity = proximity;
}

void back_up()
{
  if ( TIME_TO_DISPLAY ) {
    clear();
    lcd_goto_xy(0,0);
    print("Backing");
    lcd_goto_xy(0,1);
    print("Up");
  }
  // Back up slightly to the left
  set_motors(-90,-50);
}

void turn_in_place() {
  if (TIME_TO_DISPLAY) {
    clear();
    lcd_goto_xy(0,0);
    print("Front");
    lcd_goto_xy(0,1);
    print("Obstacle");
  }
  // Turn to the right in place
  set_motors(-50, 50);
}







