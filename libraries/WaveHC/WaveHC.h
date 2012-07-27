/*
 William Greiman's modified version of Ladyada's wave shield libary
 I have made many changes that may have introduced bugs.  Major changes are:
 wrote FatReader to read FAT32 and FAT16 files
 modified readwavhack to be readWaveData
 use standard SD and SDHC flash cards.
 skip non-data chunks after fmt chunk
 allow 18 byte format chunk if no compression
 play stereo as mono by interleaving channels
 change method of reading fmt chunk - use union of structs
*/
#ifndef WaveHC_h
#define WaveHC_h

#include "FatReader.h"

class WaveHC {
 public:
  WaveHC(void);
  uint8_t create(FatReader &f);
  uint32_t getSize(void) {return fd->fileSize();}
  uint8_t isPaused(void);
  void pause(void);
  void play(void);
  void resume(void);
  void seek(uint32_t pos);
  void setSampleRate(uint32_t samplerate);
  void stop(void);
  
  uint8_t Channels;
  uint32_t dwSamplesPerSec;
//  uint16_t wBlockAlign;
  uint8_t BitsPerSample;
  uint32_t remainingBytesInChunk;
//  uint32_t chunkSize;
  volatile uint8_t isplaying;
  uint32_t errors;
  FatReader* fd;
};

int16_t readWaveData(WaveHC *wav, uint8_t *buff, uint16_t len);

#endif //WaveHC_h
