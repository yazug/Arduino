The Arduino SdFat library provides read/write access to FAT16/FAT32
file systems on SD/SDHC flash cards.

To use this library place the uncompressed SdFat folder into the 
libraries subfolder in your main sketches folder.  You may need to
create the libraries folder.  Restart the Arduino IDE if it was open.

SdFat assumes chip select for the SD card is the hardware SS pin.  On a
168/328 Arduino this is pin 10 and on a Mega this is pin 53.  If you are
using another pin for chip select you will need call
Sd2Card::init(sckRateID, chipSelectPin) with second parameter set to the
chip select pin.  

If you have a shield like the SparkFun shield that uses pin 8 for chip
select you would change the line:
  card.init(SPI_HALF_SPEED);
to
  card.init(SPI_HALF_SPEED, 8);
in the SdFat examples.


You can also edit Sd2Card.h and change the default value for chip select.
Replace SS_PIN with the new value for chip select in the following definition.

/** The default chip select pin for the SD card is SS. */
uint8_t const  SD_CHIP_SELECT_PIN = SS_PIN;


If you wish to report bugs or have comments, send email to
fat16lib@sbcglobal.net

Read changes.txt if you have used previous releases of this library.

Read troubleshooting.txt for common hardware problems.

Please read the html documentation for this library.  Start with
html/index.html and read the Main Page.  Next go to the Classes tab and
read the documentation for the classes SdFile, SdVolume, and Sd2Card.


Arduinos access SD cards using the cards SPI protocol.  PCs, Macs, and
most consumer devices use the 4-bit parallel SD protocol.  A card that
functions well on A PC or Mac may not work well on the Arduino.

Most cards have good SPI read performance but cards vary widely in SPI
write performance.  Write performance is limited by how efficiently the
card manages internal erase/remapping operations.  The Arduino cannot
optimize writes to reduce erase operations because of its limited RAM.

SanDisk cards generally have good write performance.  They seem to have
more internal RAM buffering than other cards and therefore can limit
the number of flash erase operations that the Arduino forces due to its
limited RAM.


The hardware interface to the SD card should not use a resistor based
level shifter.  SdFat sets the SPI bus frequency to 8 MHz which results
in signal rise times that are too slow for the edge detectors in many
newer SD card controllers when resistor voltage dividers are used.

The 5 to 3.3 V level shifter for 5 V arduinos should be IC based like
the 74HC4050N based circuit shown in the file SdLevel.png.  The
Adafruit Wave Shield uses a 74AHC125N.  Gravitech sells SD and MicroSD
Card Adapters based on the 74LCX245.

If you are using a resistor based level shifter and are having problems
try setting the SPI bus frequency to 4 MHz. This can be done by using
card.init(SPI_HALF_SPEED) to initialize the SD card.

A feature to use software SPI is available.  Software SPI is slower
than hardware SPI but allows any digital pins to be used.  See
Sd2Card.h for software SPI definitions.

An unmodified Adafruit GPS shield can be use on an Arduino Mega
by defining MEGA_SOFT_SPI in Sd2Card.h.


This library was designed to be used on 328 Arduinos but smaller
applications can be written for 168 Arduinos.  This library compiles
for the Mega Arduino but has not been thoroughly tested.

For the AtMega168 be sure to reduce the serial buffer size by setting 
RX_BUFFER_SIZE to 32 or less in
hardware/cores/arduino/HardwareSerial.cpp.
I use 16.


The best way to restore an SD card's format is to use SDFormatter
which can be downloaded from:

http://www.sdcard.org/consumers/formatter/

SDFormatter aligns flash erase boundaries with file
system structures which reduces write latency and file system overhead.

SDFormatter does not have an option for FAT type so it may format
small cards as FAT12.


The SdFat/examples directory has the following sketches, they are my
debug sketches.

SdFatAnalogLogger.pde - A simple data logger for one or more analog
                        pins.

SdFatAppend.pde - This sketch creates a large file by successive
                  open/write/close operations using O_APPEND.

SdFatBench.pde - A read/write benchmark.

SdFatCopy.pde - Copy the file created by SdFatAppend.pde to the file
                ACOPY.TXT.

SdFatGPS_CSVSensorLogger.pde - Ladyada's GPS logger modified to use the
                               SdFat library.

SdFatGPSLogger_v3.pde - Ladyada's GPS logger modified to use the SdFat
                        library.

SdFatInfo.pde  - This Sketch attempts to initialize an SD card and
                 analyze its format. Used for debug problems with SD
                 cards.

SdFatLs.pde - A test of the ls() file list function.

SdFatMakeDir.pde - A debug sketch to verify subdirectory creation and
                   access.

SdFatPrint.pde - This sketch shows how to use the Arduino Print class
                 with SdFat.

SdFatRawWrite.pde - A test of raw write functions for contiguous files.

SdFatRead.pde  - This sketch reads and prints the file PRINT00.TXT
                 created by SdFatPrint.pde or WRITE00.TXT created by
                 SdFatWrite.pde.

SdFatRemove.pde - This sketch shows how to use remove() to delete the
                  file created by the SdFatAppend.pde example.

SdFatRewrite.pde - This sketch shows how to rewrite part of a line in
                   the middle of the file created by the
                   SdFatAppend.pde example.

SdFatRmDir.pde - A sketch to test file and directory deletion.


SdFatTail.pde  - This sketch reads and prints the tail of all files
                 created by SdFatAppend.pde, SdFatPrint.pde, and
                 SdFatWrite.pde.

SdFAtTimestamp.pde - This sketch shows how to set file access, create,
                     and write/modify timestamps.

SdFatTruncate.pde - This sketch shows how to use truncate() to remove
                    the last half of the file created by the
                    SdFatAppend.pde example.

SdFatWrite.pde - This sketch creates a new file and writes 100 lines
                 to the file.

To access these examples from the Arduino development environment
go to:  File -> Examples -> SdFat -> <Sketch Name>

Compile, upload to your Arduino and click on Serial Monitor to run
the example.


Updated 13 Aug 2010
