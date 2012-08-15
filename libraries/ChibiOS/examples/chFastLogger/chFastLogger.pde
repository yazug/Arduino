// Data logger optimized for 328 Arduino
// Attempt to log two ADC values every 1024 usec
//
#include <ChibiOS.h>
#include <QueueTemplate.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <BufferedWriter.h>

// sleep time between points (1024 usec per tick on a 16 MHz Arduino)
const uint8_t SLEEP_TICKS = 1;

// LED to indicate overrun error
const uint8_t OVER_LED = 3;

// SD file system
SdFat sd;
// logging file
SdBaseFile file;
// fast buffered text formatter
BufferedWriter bw;
//-----------------------------------------------------------------------------
// pack data point into three byte struct
// Ten bits for each ADC value and four bits for overrun count
struct point {
  unsigned data0 : 10;
  unsigned data1 : 10;
  unsigned over  : 4;
};
// max value for overrun count
const uint8_t MAX_OVER = 15;

// queue with 200 points
QueueTemplate<point, 200> q;
//------------------------------------------------------------------------------
// thread for sensor read - runs every SLEEP_TICKS (1024 usec per tick)
// 32 byte stack beyond task switch and interrupt needs
static WORKING_AREA(waThread1, 32);

static msg_t Thread1(void *arg) {
  uint8_t over = 0;
  while (TRUE) {
    // get pointer to next empty item
    point* h = q.headItem();
    if (h) {
      // fill in data
      h->data0 = analogRead(0);
      h->data1 = analogRead(1);
      h->over = over;
      
      // advance head to make item available to idle thread
      q.headNext();
      
      // clear overrun count
      over = 0;
    } else {
      // queue full - count overrun
      if (over < MAX_OVER) over++;
    }
    // sleep for until time for next data point (1024 usec per tick)
    chThdSleep(SLEEP_TICKS);
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
  // this is now the main thread
  
  pinMode(OVER_LED, OUTPUT);
  Serial.begin(9600);
  PgmPrintln("Type any character to start");
  while (Serial.read() < 0) {}
  
  // initialize file system
  if (!sd.init()) sd.initErrorHalt();
  
  // open or create log file
  if (!file.open("CHIBIOS.CSV", O_CREAT | O_WRITE | O_TRUNC)) {
    sd.errorHalt_P(PSTR("open"));
  }
  // initialize text formatter
  bw.init(&file);
  
  PgmPrint("FreeRam: ");
  Serial.println(FreeRam());
  
  // read any extra characters
  while(Serial.read() >= 0) {}
  PgmPrintln("Type any character to stop");
  
  // write header info
  bw.putStr_P(PSTR("Start millis: "));
  bw.putNum(millis());
  bw.putCRLF();
  bw.putStr_P(PSTR("data0,data1,overrun\r\n"));  
  if (file.writeError) sd.errorHalt_P(PSTR("write bug"));
  
  // start sensor read thread
  chThdCreateStatic(waThread1, sizeof(waThread1),
    NORMALPRIO + 2, Thread1, NULL);
}
//------------------------------------------------------------------------------
void loop() {
  uint16_t n = q.count();
  for (uint16_t i = 0; i < n; i++) {
    // get pointer to next data item
    point* t = q.tailItem();
    
    // format data point
    bw.putNum(t->data0);
    bw.putChar(',');
    bw.putNum(t->data1);
    bw.putChar(',');
    bw.putNum(t->over);
    bw.putCRLF();
    
    // light LED if overrun error
    if (t->over) digitalWrite(OVER_LED, HIGH);
    
    // free queue space for data item
    q.tailNext();
  }
  // check for SD write error
  if (file.writeError) sd.errorHalt_P(PSTR("write error"));
  
  // check for end run
  if (!Serial.available()) return;
  
  // add time
  bw.putStr_P(PSTR("End millis: "));
  bw.putNum(millis());
  bw.putCRLF();
  
  // flush buffer and close file
  bw.writeBuf();
  file.close();
  PgmPrintln("Done");
  while(1);
}
