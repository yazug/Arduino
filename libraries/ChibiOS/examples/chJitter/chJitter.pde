// test of jitter in sleep for one tick (1024 usec)
//
// idle loop prints min and max time between sleep calls
//
// Note: access to shared variables tmin and tmax is not atomic
// so glitches are possible if context switch happens during idle loop
// access to these variables.
//
#include <ChibiOS.h>

volatile  uint16_t tmax = 0;
volatile  uint16_t tmin = 0XFFFF;
//------------------------------------------------------------------------------
// thread for sleep interval
static WORKING_AREA(waThread1, 64);

static msg_t Thread1(void *arg) {
  // initialize tlast
  chThdSleep(1);
  uint32_t tlast = micros();
  
  while (TRUE) {
    chThdSleep(1);
    // get wake time
    uint32_t tmp = micros();
    uint16_t diff = tmp - tlast;
    if (diff < tmin) tmin = diff;
    if (diff > tmax) tmax = diff;
    tlast = tmp;
  }
  return 0;
}
//------------------------------------------------------------------------------
void setup() {
  cli();
  halInit();
  chSysInit();  
  Serial.begin(9600);
  chThdCreateStatic(waThread1, sizeof(waThread1), 
    NORMALPRIO + 2, Thread1, NULL);
}
uint8_t np = 0;
void loop() {
  chThdSleepMilliseconds(1000);
  Serial.print(tmin);
  Serial.write(',');
  Serial.println(tmax);
  if (np++ == 10) {
    np = 0;
    tmin = 0XFFFF;
    tmax = 0;   
    Serial.write("clear\r\n");
  }
}
