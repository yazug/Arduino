// Simple demo of three threads
// LED blink thread, print thread, and idle loop
#include <ChibiOS.h>
#include <util/atomic.h>
const uint8_t LED_PIN = 13;

volatile uint32_t count = 0;
//------------------------------------------------------------------------------
// thread 1 - high priority for blinking LED
// 64 byte stack beyond task switch and interrupt needs
static WORKING_AREA(waThread1, 64);

static msg_t Thread1(void *arg) {
  pinMode(LED_PIN, OUTPUT);
  while (TRUE) {
    digitalWrite(LED_PIN, HIGH);
    chThdSleepMilliseconds(50);
    digitalWrite(LED_PIN, LOW);
    chThdSleepMilliseconds(150);
  }
  return 0;
}
//------------------------------------------------------------------------------
// thread 2 - print idle loop count every second
// 200 byte stack beyond task switch and interrupt needs
static WORKING_AREA(waThread2, 200);

static msg_t Thread2(void *arg) {
  Serial.begin(9600);
  while (TRUE) {
    Serial.println(count);
    chThdSleepMilliseconds(1000);
  }
  return 0;
}
//------------------------------------------------------------------------------
void setup() {
  // initialize ChibiOS with interrupts disabled
  // ChibiOS will enable interrupts
  cli();
  halInit();
  chSysInit();
  
  // start blink thread
  chThdCreateStatic(waThread1, sizeof(waThread1),
    NORMALPRIO + 2, Thread1, NULL);
    
  // start print thread
  chThdCreateStatic(waThread2, sizeof(waThread2),
    NORMALPRIO + 1, Thread2, NULL);
}
//------------------------------------------------------------------------------
// idle loop runs at NORMALPRIO
void loop() {
  // must insure increment is atomic
  // in case of context switch for print
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    count++;
  }
}