/*
 William Greiman's modified version of Ladyada's wave shield libary
 I have made many changes that may have introduced bugs.  Major changes are:
 use of FatReader to read FAT32 and FAT16 files
 modified readwavhack to be readWaveData
 use standard SD and SDHC flash cards.
 skip non-data chunks after fmt chunk
 allow 18 byte format chunk if no compression
 play stereo as mono by interleaving channels
 change method of reading fmt chunk - use union of structs
*/
#include <avr/io.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "WProgram.h"
#include "FatReader.h"
#include "WaveHC.h"
#include "dac.h"
#include "WaveUtil.h"
WaveHC *playing = 0;
#define PLAYBUFFLEN 256UL
uint8_t buffer1[PLAYBUFFLEN];
uint8_t buffer2[PLAYBUFFLEN];
uint8_t *playbuff, *doublebuff;     // pointers to the current audio buffer and back buffer
uint8_t *currentpos, *endbuffpos;   // the current playing location and the end of the buffer

volatile uint8_t fillingbuffer = 0;
volatile uint8_t doublebuffready = 0;
//uint16_t temp16;

#define DEBUG 0

#define DVOLUME 0

#define OSX_BUG_FIX 0

#define SECTORSIZE 512

#if defined(__AVR_ATmega328P__)
SIGNAL(TIMER1_COMPA_vect) {
#else
SIGNAL(SIG_OUTPUT_COMPARE1A) {
#endif

#if OSX_BUG_FIX > 0
 asm volatile(
	"push r18 \n\t"
	"push r19 \n\t"
	"push r20 \n\t"
	"push r21 \n\t"
	"push r22 \n\t"
	"push r23 \n\t"
	"push r26 \n\t"
	"push r27 \n\t"
	::);
#endif //OSX_BUG_FIX

  uint8_t t8, i;

  if (!playing) 
    return;


  if (currentpos == endbuffpos) {
    if (doublebuffready) {
      // swap double buffers
      if (playbuff == buffer1) {
	playbuff = buffer2;
	doublebuff = buffer1;
      } else {
	playbuff = buffer1;
	doublebuff = buffer2;
      }
      currentpos = playbuff;
      endbuffpos = playbuff + PLAYBUFFLEN;
      doublebuffready = 0;
      if (!fillingbuffer) {
	TIMSK1 |= _BV(OCIE1B);   // fill the doublebuffer up
      }
    } else {
      playing->errors++;
      return;
    }
  }


  // ok get ready to output data to the dac
  // do the THING
  /* this is the 'wrapped' version thats all pretty
    temp16= currentpos[1];
  temp16 <<= 8;
  temp16 |= currentpos[0];
  
  // turn into 12 bit for dac
  temp16 >>= 4;
  temp16 ^= 0x800;
  dac_send_val(temp16);
  */

  
  // unwrapped loop, save some cycles!
  select_dac();

  // FIRST BIT: dac a
  dac_data_low();
  dac_clock_up();  dac_clock_down();  

  // SECOND BIT: buffered? (yes)
  dac_data_high();
  dac_clock_up(); dac_clock_down();  

  // THIRD BIT: gain = 1
  //dac_data_high();  // already high from last bit
  dac_clock_up();  dac_clock_down(); 

  // FOURTH BIT: enabled? (yes)
  //dac_data_high();  // already high from last bit
  dac_clock_up();  dac_clock_down(); 

  // Now 8 or 12 bits of data
  
  if (playing->BitsPerSample == 16) {

#if DVOLUME
    temp16= currentpos[1];
    temp16 <<= 8;
    temp16 |= currentpos[0];

    temp16 ^= 0x8000;
    temp16 >>= playing->volume;
    t8 = temp16>>8;
#else
    t8 = currentpos[1];
    t8 ^= 0x80;
#endif

    for (i=0; i<8; i++) {
      if (t8 & 0x80)
	DAC_DI_PORT |= _BV(DAC_DI);
      else
	DAC_DI_PORT &= ~_BV(DAC_DI);
      dac_clock_up();
      t8 <<= 1;
      dac_clock_down();
    }

#if DVOLUME
    t8 = temp16&0xFF;
#else
    t8 = currentpos[0];
#endif
    for (i=0; i<4; i++) {
      if (t8 & 0x80)
	DAC_DI_PORT |= _BV(DAC_DI);
      else
	DAC_DI_PORT &= ~_BV(DAC_DI);
      dac_clock_up();
      t8 <<= 1;
      dac_clock_down();
    }
    currentpos+=2; // two bytes
  } else if (playing->BitsPerSample == 8) {
    // 12 bit dac,
    t8 = currentpos[0];
    //t8 ^= 0x80;
    for (i=0; i<8; i++) {
      if (t8 & 0x80)
	DAC_DI_PORT |= _BV(DAC_DI);
      else
	DAC_DI_PORT &= ~_BV(DAC_DI);
      dac_clock_up();
      t8 <<= 1;
      dac_clock_down();
    }
    // 4 dummy bits
    dac_clock_up(); dac_clock_down();
    dac_clock_up(); dac_clock_down();
    dac_clock_up(); dac_clock_down();
    dac_clock_up(); dac_clock_down();
    currentpos++; // one byte
  }

  unselect_dac();
  dac_latch_down();
  dac_latch_up();  

#if OSX_BUG_FIX > 0
// Work-around for avr-gcc 4.3 OSX version bug
// Restore the registers that the compiler misses
  asm volatile(
	"pop r27 \n\t"
	"pop r26 \n\t"
	"pop r23 \n\t"
	"pop r22 \n\t"
	"pop r21 \n\t"
	"pop r20 \n\t"
	"pop r19 \n\t"
	"pop r18 \n\t"
	::);  
#endif //OSX_BUG_FIX	
}

// this is the interrupt that fills the playbuffer
#if defined(__AVR_ATmega328P__)
SIGNAL(TIMER1_COMPB_vect) {
#else
SIGNAL(SIG_OUTPUT_COMPARE1B) {
#endif

#if OSX_BUG_FIX > 0
  asm volatile(
	"push r18 \n\t"
	"push r19 \n\t"
	"push r20 \n\t"
	"push r21 \n\t"
	"push r22 \n\t"
	"push r23 \n\t"
	"push r26 \n\t"
	"push r27 \n\t"
	::);
#endif //OSX_BUG_FIX	
	
  uint16_t read;

  TIMSK1 &= ~_BV(OCIE1B);   // turn off bufferfiller 
  if (doublebuffready) { // we're not needed ??
    return;
  }

  fillingbuffer = 1;   // we're doing stuff, quit buggin

  sei();

  read = readWaveData(playing, doublebuff, PLAYBUFFLEN);

  if (read == 0) {
    playing->stop();
  }
  cli();
  fillingbuffer = 0;
  doublebuffready = 1;
  sei();
  
#if OSX_BUG_FIX > 0
// Work-around for avr-gcc 4.3 OSX version bug
// Restore the registers that the compiler misses
  asm volatile(
	"pop r27 \n\t"
	"pop r26 \n\t"
	"pop r23 \n\t"
	"pop r22 \n\t"
	"pop r21 \n\t"
	"pop r20 \n\t"
	"pop r19 \n\t"
	"pop r18 \n\t"
	::);
#endif //OSX_BUG_FIX
}

WaveHC::WaveHC(void) {
}

uint8_t WaveHC::create(FatReader &f)
{
  // 18 byte buffer
  // can use this since Arduino and RIFF are Little Endian
  union {
    struct {
      char     id[4];
      uint32_t size;
      char     data[4];
    } riff;  //start of riff chunk
    struct {
      uint16_t compress;
      uint16_t channels;
      uint32_t sampleRate;
      uint32_t bytesPerSecond;
      uint16_t blockAlign;
      uint16_t bitsPerSample;
      uint16_t extraBytes;
    } fmt; //fmt data
  } buf;

  if (f.read((uint8_t *)&buf, 12) != 12
      || strncmp(buf.riff.id, "RIFF", 4)
      || strncmp(buf.riff.data, "WAVE", 4)) {
#if DEBUG > 0
        putstring_nl("Not RIFF");
#endif
        return 0;
  }

  if (f.read((uint8_t *)&buf, 8) != 8
      || strncmp(buf.riff.id, "fmt ", 4)) {
#if DEBUG > 0
    putstring_nl("1st chunk not fmt");
#endif
        return 0;
  }
  uint16_t size = buf.riff.size;
  if (size != 16 && size != 18) {
    putstring_nl("Compression not supported");
    return 0;
  }

  if (f.read((uint8_t *)&buf, size) != (int16_t)size) {
#if DEBUG > 0
    putstring("\n\rbad fmt chunck");
#endif
    return 0;
  }
#if DEBUG
  putstring("\n\rwFormat="); Serial.println(buf.fmt.compress, HEX);
#endif
  if (buf.fmt.compress != 1 || (size == 18 && buf.fmt.extraBytes != 0)) {
    putstring_nl("Compression not supported");
    return 0;
  }
  Channels = buf.fmt.channels;
#if DEBUG > 0
    putstring("\n\rChans="); Serial.println(Channels, DEC);
#endif
  if (Channels > 2) {
    putstring_nl("Not mono/stereo!");
    return 0; // only mono/stereo!
  }
  dwSamplesPerSec = buf.fmt.sampleRate;
#if DEBUG > 0
  putstring("\n\rFreq="); Serial.println(dwSamplesPerSec, DEC);
  putstring("\n\rwBlockAlign="); Serial.println(buf.fmt.blockAlign, DEC);
#endif

  BitsPerSample = buf.fmt.bitsPerSample;
#if DEBUG > 0
  putstring("\n\rwBitSample="); Serial.println(BitsPerSample, DEC);
#endif
  if (BitsPerSample > 16) {
    putstring_nl("More than 16 bits per sample!");
    return 0; //wack!
  }

  uint8_t tooFast = 0; // flag
  if (dwSamplesPerSec > 22050) {
    // ie 44khz
    if ((BitsPerSample > 8) || (Channels > 1))
      tooFast = 1;
  } else if (dwSamplesPerSec > 16000) {
    // ie 22khz. can only do 16-bit mono or 8-bit stereo
    if ((BitsPerSample > 8) && (Channels > 1))
      tooFast = 1;
  }
//  putstring_nl("");
  if (tooFast) {
    putstring_nl("Sample rate too high!");
    return 0;
  }

  remainingBytesInChunk = 0;
  fd = &f;
  errors = 0;

  isplaying = 0;

  // ok good now onto some goddamn data
  return 1;
}
// return pause status
uint8_t WaveHC::isPaused(void)
{
  cli();
  uint8_t rtn = isplaying && !(TIMSK1 & _BV(OCIE1A));
  sei();
  return rtn;
}
// pause
void WaveHC::pause(void)
{
  cli();
  TIMSK1 &= ~_BV(OCIE1A); //disable DAC interrupt
  sei();
  fd->volume()->rawDevice()->readEnd(); // redo any partial read on resume
}

void WaveHC::play(void) {
  // setup the interrupt as necessary
  // fix for stereo - play interleaved
  // uint32_t ticksPerSample = F_CPU / dwSamplesPerSec;
  uint32_t ticksPerSample = F_CPU / (dwSamplesPerSec*Channels);
  int16_t read;

  playing = this;

  // fill the buffer so that we're on a boundary.
  //putstring("\n\rCurrent pos: "); 
  //uart_putdw_dec(wav->fd->pos);
  
  // kickstart
  currentpos = buffer1;
  read = readWaveData(playing, buffer1, 2);
  if (read <= 0)
    return;
  endbuffpos = currentpos+2; 
  read = readWaveData(playing, buffer1+2, PLAYBUFFLEN-(fd->readPosition() % PLAYBUFFLEN));
  if (read <= 0)
   return;
  endbuffpos += read;

  // fill the double buffer
  read = readWaveData(playing, buffer2, PLAYBUFFLEN);
  doublebuffready = 1;

  //putstring("\n\rNow pos: "); uart_putdw_dec(wav->fd->pos);
  
  // its official!
  isplaying = 1;

  TCCR1A = 0;              // no pwm
  TCCR1B = _BV(WGM12) | _BV(CS10); // no clock div, CTC mode
  OCR1A = ticksPerSample; // make it go off 1ce per sample, no more than 22khz
  OCR1B = 1;
  TIMSK1 |= _BV(OCIE1A);   // turn it on  

  sei();
}


int16_t readWaveData(WaveHC *wav, uint8_t *buff, uint16_t len) {
  uint8_t headerbuff[5];
#if DEBUG > 1
  putstring("*hacK "); uart_putdw_dec(len); putstring_nl("");
#endif
  if (wav->remainingBytesInChunk == 0) {
    while (1) {
      // read chunk ID
      if (wav->fd->read(headerbuff, 4) != 4) return 0;
      headerbuff[4] = 0;
      if (wav->fd->read((uint8_t *)&wav->remainingBytesInChunk, 4) != 4) return 0;
#if DEBUG > 0
      Serial.print((char *)headerbuff);
      putstring(" type, size=");
      Serial.print(wav->remainingBytesInChunk, DEC);
      putstring_nl("");
#endif

      if (!strncmp((char *)headerbuff, "data", 4)) break;
      // MEME, if not "data" then skip it!
      if (!wav->fd->seekCur(wav->remainingBytesInChunk)) return 0;
    }
  }

  if (len > SECTORSIZE) len = SECTORSIZE;

  if (len > wav->remainingBytesInChunk) {
    len = wav->remainingBytesInChunk;
#if DEBUG > 0
    putstring(" @@@ ");
#endif
  }
  
  int16_t ret;
  ret = wav->fd->read(buff, len);
  
  if (ret != (int16_t)len) {
#if DEBUG > 0
    putstring(" ### ");
#endif
    return ret;
  }
  
  wav->remainingBytesInChunk -= len;
  return len;
}

void WaveHC::resume(void)
{
  cli();
  // enable DAC interrupt
  if(isplaying) TIMSK1 |= _BV(OCIE1A);
  sei();
}

void WaveHC::seek(uint32_t pos)
{
  pos -= pos % PLAYBUFFLEN;
  if (pos < PLAYBUFFLEN) pos = PLAYBUFFLEN; //don't play metadata
  if (pos > fd->fileSize()) pos = fd->fileSize();
  cli();  
  if (isplaying) {
    if (fd->seekSet(pos)) {
      remainingBytesInChunk = fd->fileSize() - pos;
    }
  }
  sei();
}
void WaveHC::setSampleRate(uint32_t samplerate) 
{
    while (TCNT0 != 0);
     OCR1A = F_CPU / samplerate;
}

void WaveHC::stop(void) {
  TIMSK1 &= ~_BV(OCIE1A);   // turn on buferfixer if not
#if DEBUG > 0
  putstring("\n\rAll done!\n\r"); // MEME: Fix last bytes
  Serial.print(playing->errors, DEC);
  putstring_nl(" errors");
#endif
  playing->isplaying = 0;
  playing = 0;
}
