// example optimized for Mega
// attempts to log four ADC values every tick (1024 usec)
// will run on 328 Arduinos with one point every four ticks
#include <ChibiOS.h>
#include <QueueTemplate.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <BufferedWriter.h>

// number of ADCs to read for each point
const uint8_t ADC_COUNT = 4;

#if RAMEND > 4000
// its a Mega so try for a data point every tick (1024 usec)
const uint8_t QUEUE_SIZE = 250;
const uint8_t SLEEP_TICKS = 1;
#else  // RAMEND
// 328 so not as much RAM try for a point every 4096 usec
const uint8_t QUEUE_SIZE = 50;
const uint8_t SLEEP_TICKS = 4;
#endif  // RAMEND

// SD file system
SdFat sd;

//logging file
SdBaseFile file;

// fast buffered text formatter
BufferedWriter bw;

// LED to indicate overruns
const uint8_t OVER_LED = 3;
//-----------------------------------------------------------------------------
// data structure for data point
struct point {
  uint16_t data[ADC_COUNT];
  uint8_t over;
};
// max value for overrun count
const uint8_t MAX_OVER = 255;

// queue for buffering data points
QueueTemplate<point, QUEUE_SIZE> q;
//------------------------------------------------------------------------------
// thread for sensor read - runs SLEEP_TICKS (1024 usec per tick)
// 64 byte stack beyond task switch and interrupt needs
static WORKING_AREA(waThread1, 64);

static msg_t Thread1(void *arg) {
  uint8_t over = 0;
  while (TRUE) {
    // get pointer to next free data point
    point* h = q.headItem();
    if (h) {
      for (uint8_t i = 0; i < ADC_COUNT; i++) {
        h->data[i] = analogRead(i);
      }
      h->over = over;
      // advance head so writer will see point
      q.headNext();
      
      // clear overrun count
      over = 0;
    } else {
      // count overrun - queue full
      if (over < MAX_OVER) over++;
    }
    // sleep until time for next point (1024 usec per tick) 
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
  // this is now the idle thread
  
  pinMode(OVER_LED, OUTPUT);
  Serial.begin(9600);
  PgmPrintln("Type any character to start");
  while (Serial.read() < 0) {}
  
  if (!sd.init()) sd.initErrorHalt();
  // open or create log file
  if (!file.open("CHIBIOS.CSV", O_CREAT | O_WRITE | O_TRUNC)) {
    sd.errorHalt_P(PSTR("open"));
  }
  // initial fast buffered formatter
  bw.init(&file);
  
  // Show amount of free RAM
  PgmPrint("FreeRam: ");
  Serial.println(FreeRam());
  
  // read any extra characters from Serial
  while(Serial.read() >= 0) {}
  PgmPrintln("Type any character to stop");

  // write header info to file
  bw.putStr_P(PSTR("Start millis: "));
  bw.putNum(millis());
  bw.putCRLF();
  for (uint8_t i = 0; i < ADC_COUNT; i++) {
    bw.putStr_P(PSTR("data"));
    bw.putNum(i);
    bw.putChar(',');
  }
  bw.putStr_P(PSTR("over\r\n"));
  
  // start sensor read thread
  chThdCreateStatic(waThread1, sizeof(waThread1),
    NORMALPRIO + 2, Thread1, NULL);
}
//--------------------------------------------------------------------------------------
void loop() {
  uint16_t n = q.count();
  for (uint16_t i = 0; i < n; i++) {
    // get pointer to next data point
    point* p = q.tailItem();
    
    // write data point
    for (uint8_t i = 0; i < ADC_COUNT; i++) {
      bw.putNum(p->data[i]);
      bw.putChar(',');
    }
    bw.putNum(p->over);
    bw.putCRLF();
      
    // if overrun light LED
    if (p->over) digitalWrite(OVER_LED, HIGH);    
    
    // free data point storage  in queue
    q.tailNext();
  }
  // check for write error
  if (file.writeError) sd.errorHalt_P(PSTR("write error"));

  // end run if Serial data typed
  if (!Serial.available()) return;
  
  // record end millis
  bw.putStr_P(PSTR("End millis: "));
  bw.putNum(millis());
  bw.putCRLF();
  
  // flush buffer and close file
  bw.writeBuf();
  file.close();
  PgmPrintln("Done");
  while(1);
}
