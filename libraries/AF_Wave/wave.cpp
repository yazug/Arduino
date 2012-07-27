#include <avr/io.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "WProgram.h"
#include "partition.h"
#include "sd_raw.h"
#include "sd_raw_config.h"
#include "fat16.h"
#include "fat16_config.h"
#include "wave.h"
#include "dac.h"
#include "fat16.h"
#include "util.h"
#include "AF_Wave.h"

Wavefile *playing = 0;
#define PLAYBUFFLEN 256UL
uint8_t buffer1[PLAYBUFFLEN];
uint8_t buffer2[PLAYBUFFLEN];
uint8_t *playbuff, *doublebuff;     // pointers to the current audio buffer and back buffer
uint8_t *currentpos, *endbuffpos;   // the current playing location and the end of the buffer

volatile uint8_t fillingbuffer = 0;
volatile uint8_t doublebuffready = 0;
uint16_t temp16;

#define DEBUG 0

#define DVOLUME 0

#if defined(__AVR_ATmega328P__)
SIGNAL(TIMER1_COMPA_vect) {
#else
SIGNAL(SIG_OUTPUT_COMPARE1A) {
#endif
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
}


// this is the interrupt that fills the playbuffer
#if defined(__AVR_ATmega328P__)
SIGNAL(TIMER1_COMPB_vect) {
#else
SIGNAL(SIG_OUTPUT_COMPARE1B) {
#endif

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
	
	
  uint16_t read;

  TIMSK1 &= ~_BV(OCIE1B);   // turn off bufferfiller 
  if (doublebuffready) { // we're not needed ??
    return;
  }

  fillingbuffer = 1;   // we're doing stuff, quit buggin

  sei();

  read = readwavhack(playing, doublebuff, PLAYBUFFLEN);

  if (read == 0) {
    playing->stop();
  }
  cli();
  fillingbuffer = 0;
  doublebuffready = 1;
  sei();
  
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
}

Wavefile::Wavefile(void) {
}

void Wavefile::play(void) {
  // setup the interrupt as necessary
  uint32_t ticksPerSample = F_CPU / dwSamplesPerSec;
  uint16_t read;

  playing = this;

  // fill the buffer so that we're on a boundary.
  //putstring("\n\rCurrent pos: "); 
  //uart_putdw_dec(wav->fd->pos);
  
  // kickstart
  currentpos = buffer1;
  read = readwavhack(playing, buffer1, 2);
  if (read == 0)
    return;
  endbuffpos = currentpos+2; 
  read = readwavhack(playing, buffer1+2, PLAYBUFFLEN-(fd->pos % PLAYBUFFLEN));
  if (read == 0)
   return;
  endbuffpos += read;

  // fill the double buffer
  read = readwavhack(playing, buffer2, PLAYBUFFLEN);
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

#define SECTORSIZE 512 

uint16_t readwavhack(Wavefile *wav, uint8_t *buff, uint16_t len) {
  uint8_t headerbuff[5];
#if DEBUG > 1
  putstring("*hacK "); uart_putdw_dec(len); putstring_nl("");
#endif
  if (wav->remainingBytesInChunk == 0) {
    // read chunk ID
    if (fat16_read_file(wav->fd, headerbuff, 4) != 4)
      return 0;
    headerbuff[4] = 0;

#if DEBUG > 1
    uart_puts((char *)headerbuff);
#endif
    // MEME, if not "data" then skip it!
    
    if (fat16_read_file(wav->fd, headerbuff, 4) != 4)
      return 0;
    wav->remainingBytesInChunk = 0;
    for(int8_t i = 3; i >= 0; i--) {
      wav->remainingBytesInChunk <<= 8;
      wav->remainingBytesInChunk += headerbuff[i];
    }
    
#if DEBUG > 0
    putstring(" type, size="); 
    Serial.print(wav->remainingBytesInChunk, DEC);
    putstring_nl("");
#endif
  }

  if (len > SECTORSIZE)
    len = SECTORSIZE;

  if (len > wav->remainingBytesInChunk) {
    len = wav->remainingBytesInChunk;
#if DEBUG > 0
    putstring(" @@@ ");
#endif
  }

  uint16_t ret;

  if ((wav->fd->pos % SECTORSIZE == 0) && !wav->inmiddlesector) { 
#if DEBUG > 1
    putstring("starting sector...");
#endif
    ret = fat16_start_readsector(wav->fd, buff, len);
    if (ret != len) {
      putstring(" ??? ");
      sd_raw_read_end();
      return ret;
    }
#if DEBUG > 1
    putstring_nl("OK");
    putstring("ADDR = "); uart_putdw_hex(wav->fd->pos);  putstring_nl("");
#endif
    wav->inmiddlesector = 1;
  } else if (wav->inmiddlesector) {
    ret = sd_raw_read_more(buff, len);
#if DEBUG > 1
    putstring(", read more..."); uart_putw_dec(ret);    
#endif
    if (ret != len) {
      putstring(" !!! ");
      sd_raw_read_end();
    } 
#if DEBUG > 1
    putstring_nl(" OK");
#endif
    len = ret;
    wav->fd->pos += len;
    //    putstring("ADDR = "); uart_putdw_hex(wav->fd->pos);  putstring_nl("");
    if (wav->fd->pos % SECTORSIZE == 0) {
      sd_raw_read_end();
#if DEBUG > 1
      putstring_nl(" sector done");
#endif
      wav->inmiddlesector = 0;
      
      if (wav->fd->pos % wav->fd->fs->header.cluster_size == 0) {
	/* we are on a cluster boundary, so get the next cluster */
	if(! (wav->fd->pos_cluster = fat16_get_next_cluster(wav->fd->fs, wav->fd->pos_cluster)))
	  {
	    wav->fd->pos_cluster = 0;
	    putstring_nl("failed to get new cluster");
	    return (uint16_t)-1;	
	  }
	//putstring("**********************new cluster "); uart_putw_hex(wav->fd->pos_cluster);
      }
    }
  } else {
    if ((unsigned)fat16_read_file(wav->fd, buff, len) != len)
      return 0;
  }
  wav->remainingBytesInChunk -= len;
  
  return len;
}


uint8_t Wavefile::create(File f) {
  uint8_t buffer[8];
  uint32_t temp;
  int8_t i;
 
  if ((fat16_read_file(f, buffer, 4) != 4) ||
      (strncmp((char *)buffer, "RIFF", 4))) {
#if DEBUG > 0
    putstring("Not RIFF");
#endif
    return 0;
  }

  // read "WAVE"
  if ((fat16_read_file(f, buffer, 8) != 8) ||
      (strncmp((char *)buffer+4, "WAVE", 4))) {
#if DEBUG > 0
    putstring("Not WAVE");
#endif
    return 0;
  }
#if DEBUG > 0
  putstring_nl("found wave");
#endif
  // read format chunk ID
  if ((fat16_read_file(f, buffer, 4) != 4) ||
      (strncmp((char *)buffer, "fmt ", 4))) {
#if DEBUG > 0
    putstring_nl("1st chunk not fmt");
#endif
    return 0;
  }

 // read chunk size
  chunkSize = 0;
  if (fat16_read_file(f, buffer, 4) != 4)
    return 0;
  for(i = 3; i >= 0; i--) {
    chunkSize <<= 8;
    chunkSize += buffer[i];
  }
#if DEBUG > 0
  putstring("\n\rchunk size: ");
  Serial.println(chunkSize, DEC);
#endif
  if (chunkSize != 16) {
#if DEBUG > 0
    putstring_nl("Compression not supported");
#endif
    return 0;
  }

  if (fat16_read_file(f, buffer, 8) != 8)
    return 0;
  
  temp = buffer[1]; temp <<= 8; temp += buffer[0];
#if DEBUG
  putstring("\n\rwFormat="); Serial.println(temp, HEX);
#endif
  if (temp != 1) {
    putstring_nl("Compression not supported");
    return 0;
  }

  temp = buffer[3]; temp <<= 8; temp += buffer[2];
  if (temp > 2) 
    return 0; // only mono/stereo!

  Channels = temp;
#if DEBUG > 0
  putstring("\n\rChans="); Serial.println(Channels, DEC);
#endif

  for(i = 7; i >= 4; i--) {
    temp <<= 8;
    temp += buffer[i];
  }
  dwSamplesPerSec = temp;
#if DEBUG > 0
  putstring("\n\rFreq="); Serial.println(dwSamplesPerSec, DEC);
#endif
  if (fat16_read_file(f, buffer, 8) != 8)
    return 0;
  temp = buffer[5]; temp <<= 8; temp += buffer[4];
  wBlockAlign = temp;
#if DEBUG
  putstring("\n\rwBlockAlign="); Serial.println(wBlockAlign, DEC);
#endif

  temp = buffer[7]; temp <<= 8; temp += buffer[6];
  if (temp > 16)
    return 0; //wack!
  BitsPerSample = temp;
#if DEBUG > 0
  putstring("\n\rwBitSample="); Serial.println(BitsPerSample, DEC);
#endif

  i = 0; // flag
  if (dwSamplesPerSec > 22050) {
    // ie 44khz
    if ((BitsPerSample > 8) || (Channels > 1)) 
      i = 1;
  } else if (dwSamplesPerSec > 16000) { 
    // ie 22khz. can only do 16-bit mono or 8-bit stereo
    if ((BitsPerSample > 8) && (Channels > 1))
      i = 1;
  }
  putstring_nl("");
  if (i) {
    putstring_nl("Sample rate too high!");
    return 0;
  }

  remainingBytesInChunk = 0;
  fd = f;
  errors = 0;

  isplaying = 0;

  inmiddlesector = 0;

  volume = 0;      // 0 volume is actually max

  // ok good now onto some goddamn data
  return 1;
}


void Wavefile::stop(void) {
  TIMSK1 &= ~_BV(OCIE1A);   // turn on buferfixer if not
  if (inmiddlesector){
    sd_raw_read_end();
    inmiddlesector = 0;
  }
  #if DEBUG > 0
  putstring("\n\rAll done!\n\r"); // MEME: Fix last bytes
  Serial.print(playing->errors, DEC);
  putstring_nl(" errors");
  #endif
  playing->isplaying = 0;
  playing = 0;
}


void Wavefile::seek(int32_t location) {
  location -= location % PLAYBUFFLEN;
  if (isplaying) {
    cli();
    if (inmiddlesector){
      sd_raw_read_end();
      inmiddlesector = 0;
    }
  }
  fat16_seek_file(fd, &location, FAT16_SEEK_SET);
  remainingBytesInChunk = getSize() - location;
  sei();
}

uint32_t Wavefile::getSize(void) {
  return fd->dir_entry.file_size;
}

void Wavefile::setSampleRate(uint32_t samplerate) {
    while (TCNT0 != 0);
     OCR1A = F_CPU / samplerate;
}
