#include <FatReader.h>
#include <SdReader.h>
#include <avr/pgmspace.h>
#include "WaveUtil.h"
#include "WaveHC.h"

SdReader card;
FatVolume vol;
FatReader root;
uint8_t dirLevel; // indent level for file/dir names
dir_t dirBuf;     // buffer for directory reads
WaveHC wave;      // only one!


// this function will return the number of bytes currently free in RAM      
int freeRam(void)
{
  extern int  __bss_end; 
  extern int  *__brkval; 
  int free_memory; 
  if((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end); 
  }
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval); 
  }
  return free_memory; 
} 

/*
 * print error message and halt if SD I/O error
 */
void sdErrorCheck(void)
{
  if (!card.errorCode()) return;
  putstring("\n\rSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  putstring(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}
/*
 * print dir_t name field
 */
void printName(dir_t &dir)
{
  for (uint8_t i = 0; i < 11; i++) {
    if (dir.name[i] == ' ')continue;
    if (i == 8) Serial.print('.');
    Serial.print(dir.name[i]);
  }
  if (DIR_IS_SUBDIR(dir)) Serial.print('/');
}
/*
 * list recursively - possible stack overflow if subdirectories too nested
 */
void lsR(FatReader &d)
{
  int8_t r;
  while ((r = d.readDir(dirBuf)) > 0) {
    // skip subdirs . and ..
    if (dirBuf.name[0] == '.') continue;
    for (uint8_t i = 0; i < dirLevel; i++) Serial.print(' ');
    printName(dirBuf);
    Serial.println();
    if (DIR_IS_SUBDIR(dirBuf)) {
      FatReader s;
      dirLevel += 2;
      if (s.open(vol, dirBuf)) lsR(s);
      dirLevel -=2;
    }
  }
  sdErrorCheck();
}
/*
 * play recursively - possible stack overflow if subdirectories too nested
 */
void play(FatReader &dir)
{
  FatReader file;
  while (dir.readDir(dirBuf) > 0) {
    
    // skip . and .. directories
    if (dirBuf.name[0] == '.') continue;
    Serial.println();
    for (uint8_t i = 0; i < dirLevel; i++) Serial.print(' ');
    if (!file.open(vol, dirBuf)) {
      Serial.println("file.open failed");
      while(1); 
    }
    
    if (file.isDir()) {
      putstring("Subdir: ");printName(dirBuf);
      dirLevel += 2;
      // play files in subdirectory
      play(file);
      dirLevel -= 2;    
    }
    else {
      putstring("Playing ");printName(dirBuf);
      if (!wave.create(file)) {
        putstring(" Not a valid WAV");
      }
      else {
        Serial.println();
        wave.play();
       
        while (wave.isplaying) {
          putstring(".");
          delay(100);
        }
        sdErrorCheck();
//        if (wave.errors)Serial.println(wave.errors);
      }
    }
  }
}

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  putstring_nl("\nWave test!");
  putstring("Free RAM: ");
  Serial.println(freeRam());  
 
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
//  if (!card.init(true)) {//play with 4 MHz spi
  if (!card.init()) {//play with 8 MHz spi  
    putstring_nl("Card init. failed!");
    sdErrorCheck();while(1);
  }
  // enable optimize read - some cards may timeout
  card.partialBlockRead(true);
  
  uint8_t part;
  for (part = 0; part < 5; part++) {
    if (vol.init(card, part)) break;
  }
  if (part == 5) {
    putstring_nl("No valid FAT partition!");
    sdErrorCheck();while(1);
  }
  putstring("Using partition ");
  Serial.print(part, DEC);
  putstring(", type is FAT");
  Serial.println(vol.fatType(),DEC);
  
  if (!root.openRoot(vol)) {
    putstring_nl("Can't open root dir!"); while(1);
  }
  putstring_nl("Files found:");

  dirLevel = 0;
  lsR(root);
}

void loop() { 
  root.rewind();
  play(root);
}
