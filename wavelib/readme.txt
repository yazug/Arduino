WaveHC is designed to support Fat16 or FAT32 file systems on standard SD 
and high capacity SDHC flash cards.

It runs on both 168 and 328 Arduinos.

This library seems to work with most standard SD cards but fails with some
SDHC cards on the Adafruit Wave Shield.  This appears to be due to the
slow rise time for SPI signals.   The Wave Shield was not designed
for SDHC cards. 

I have modified a wave shield to use a 74HC4050N IC instead of the resistor
voltage divider on the wave shield to fix this problem.  

See Ladyada's forum for details. search for 74HC4050N.

If you get errors with a card please run SdReadTest.pde to get more info.

Also try replacing this line in dap_hc

if (!card.init()) {//play with 8 MHz spi 

with 

if (!card.init(true)) {//play with 4 MHz spi 

Ladyada's OSX bug fix can be activated by setting OSX_BUG_FIX to 1 in WaveHC.cpp

You can send comments to fat16lib@sbcglobal.net


Start by getting the Wave Shield working with the Adafruit AF_Wave library.

http://www.ladyada.net/make/waveshield/download.html

Make sure the examples dap.pde and pispeak.pde work. Download them at:
http://www.ladyada.net/make/waveshield/examples.html

This library, WaveHC, contains a modified version of Ladyada's programs for
playing WAV files in WaveHC.h and WaveHC.cpp.  New programs have been written
to read files from SD cards.  

For the AtMega168 be sure to reduce the serial buffer size by setting 
RX_BUFFER_SIZE to 32 or less in hardware/cores/arduino/HardwareSerial.cpp.  
I use 16.

You can free more flash by setting SD_CARD_INFO_SUPPORT to 0 in SdReader.h 
if you are not using cardSize(), or are not using the test sketch SdReadTest.pde.

Most sketches will require about 3K fewer bytes of flash when using this library.

The main modifications are:

Support for both FAT16 and FAT32 file systems.
Ability to read both standard SD cards and high capacity SDHC cards.

Other changes:
 modified readwavhack to be readWaveData.
 skip non-data chunks after fmt chunk.
 allow 18 byte fmt chunk if no compression.
 play stereo as mono by interleaving channels.
 change method of reading fmt chunk - use union of structs.

This library has been developed with Arduino 15 and may have problems
with earlier versions.

To use this library place the uncompressed WaveHC folder into the 
hardware/libraries folder (in your Arduino installation).

There will be several false warnings about program memory variables.  
This is a typical case:

WaveHC.cpp:306: warning: only initialized variables can be placed into program memory area

Try the example sketch dap_hc.pde.  It should play all WAV files on a SD
or SDHC card.  The card can be formatted as FAT16 or FAT32. 

dap_hc.pde will look in the root directory and all subdirectories.

pispeak_hc.pde should work like Ladyada's pispeak.pde.

Please read the documentation for FatReader in the html directory or pdf file.

