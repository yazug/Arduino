This is a port of ChibOS/RT for Arduino.

It is intended for users with experience using real-time kernels on
small processors.  Bugs are likely and there is little documentation.

For ChibiOS/RT documentation go to:

http://www.chibios.org/dokuwiki/doku.php?id=start

Changes to port ChibiOS/RT to the Arduino are described in ChibiOS_notes.txt.

TIMER0_COMPA_vect is used for the system tick since the Arduino already
uses timer 0 for millis() and micros().  The tick time is 1024 usec.  You
should avoid using PWM on the pin associated with timer 0 COMPA.  This
is pin 6 on a 328 and pin 13 on a Mega.

To run the ChibiOS/RT examples, copy libraries to the libraries folder
in your sketchbook folder.

All examples require the ChibiOS library. 

The chFastLogger and chMegaLogger examples require the BufferedWriter,
QueueTemplate, and SdFat libraries.

For SdFat documentation download the latest here:

http://code.google.com/p/sdfatlib/downloads/list

QueueTemplate is a C++ queue that provides thread safe buffering for the
data logger examples.  The only documentation is comments in the source.

BufferedWriter is an optimized text formatter that is 3 - 4 times faster
than the Arduino Print class.  The only documentation is comments in
the source.

There are six examples in the ChibiOS/examples folder.

chBlinkPrint - A simple example of three threads. A high priority thread
               blinks an LED, a medium priority thread prints a counter
               every second, and the low priority idle loop increments the
               counter.

chContextTime - Test to determine context switch time with a semaphore.

chFastLogger - Data logger optimized for 328 Arduino.  Logs two analog pins
               every tick (1024 usec tick).

chJitter - Test of jitter in sleep for one tick.

chMegaLogger - Data logger optimized for Mega.  Logs four analog pins
               every tick (976 samples per sec, four values per sample)

chStackUse - A version of chBlinkPrint that prints stack use for each thread