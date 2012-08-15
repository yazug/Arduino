// scope test to determine ChibiOS context switch time
#include <ChibiOS.h>

const uint8_t LED_PIN = 3;
// semaphore to trigger thread2
Semaphore sem;
//------------------------------------------------------------------------------
// thread for LED off
static WORKING_AREA(waThread1, 64);

static msg_t Thread1(void *arg) {
  while (TRUE) {
    chSemWait(&sem);
    digitalWrite(LED_PIN, LOW);
  }
  return 0;
}
//------------------------------------------------------------------------------
// thread 2 - cause context switch to thread 1 with semaphore
static WORKING_AREA(waThread2, 64);

static msg_t Thread2(void *arg) {
  while (TRUE) {
    // toggle LED to get time for digitalWrite
    digitalWrite(LED_PIN, HIGH);    
    digitalWrite(LED_PIN, LOW);
    digitalWrite(LED_PIN, HIGH);
    // signal to switch to thread1 to get context switch time
    chSemSignal(&sem);
    // wait 10 ms
    chThdSleepMilliseconds(10);
  }
  return 0;
}
//------------------------------------------------------------------------------
void setup() {
  pinMode(LED_PIN, OUTPUT);
  halInit();
  chSysInit();
  chSemInit(&sem, 0);
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO + 2, Thread1, NULL);
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO + 1, Thread2, NULL);  
  while(TRUE){}
}
void loop() {
  // never called
}

