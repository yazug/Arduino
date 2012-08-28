#ifndef	DES_h
#define	DES_h

//#include <stddef.h>
//#include <stdint.h>
//#include <stdbool.h>

#include <avr/pgmspace.h>

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "Wprogram.h"
#endif

class Des {

/*
 * The s-box values are permuted according to the 'primitive function P'
 * and are rotated one bit to the left.
 */
PROGMEM static const prog_uint32_t sbox1[64];
PROGMEM static const prog_uint32_t sbox2[64];
PROGMEM static const prog_uint32_t sbox3[64];
PROGMEM static const prog_uint32_t sbox4[64];
PROGMEM static const prog_uint32_t sbox5[64];
PROGMEM static const prog_uint32_t sbox6[64];
PROGMEM static const prog_uint32_t sbox7[64];
PROGMEM static const prog_uint32_t sbox8[64];
  
/*
 * These two tables are part of the 'permuted choice 1' function.
 * In this implementation several speed improvements are done.
 */

  PROGMEM static const prog_uint32_t leftkey_swap[16];
  PROGMEM static const prog_uint32_t rightkey_swap[16];

/*
 * Numbers of left shifts per round for encryption subkeys.  To
 * calculate the decryption subkeys we just reverse the ordering of
 * the calculated encryption subkeys, so there is no need for a
 * decryption rotate tab.
 */

  static const byte encrypt_rotate_tab[16];

  uint32_t encrypt_subkeys[32];
  uint32_t decrypt_subkeys[32];
  

private:
  /*
   * des_key_schedule():	  Calculate 16 subkeys pairs (even/odd) for
   *			  16 encryption rounds.
   *			  To calculate subkeys for decryption the caller
   *			  have to reorder the generated subkeys.
   *
   *    rawkey:	    8 Bytes of key data
   *    subkey:	    Array of at least 32 uint32_ts. Will be filled
   *		    with calculated subkeys.
   *
   */

  /*
   * Macro to swap bits across two words.
   */


#define DO_PERMUTATION(a, temp, b, offset, mask) \
  temp = ((a>>offset) ^ b) & mask; \
  b ^= temp;  \
  a ^= temp<<offset; 

/*
 * This performs the 'initial permutation' of the data to be encrypted
 * or decrypted. Additionally the resulting two words are rotated one bit
 * to the left.
 */

#define INITIAL_PERMUTATION(left, temp, right)		\
DO_PERMUTATION(left, temp, right, 4, 0x0f0f0f0f)	\
DO_PERMUTATION(left, temp, right, 16, 0x0000ffff)	\
DO_PERMUTATION(right, temp, left, 2, 0x33333333)	\
DO_PERMUTATION(right, temp, left, 8, 0x00ff00ff)	\
right =  (right << 1) | (right >> 31);		\
temp  =  (left ^ right) & 0xaaaaaaaa;		\
right ^= temp;					\
left  ^= temp;					\
left  =  (left << 1) | (left >> 31);

static void initial_permutation(uint32_t &left, uint32_t &temp, uint32_t &right) {
  DO_PERMUTATION(left, temp, right, 4, 0x0f0f0f0f)	
  DO_PERMUTATION(left, temp, right, 16, 0x0000ffff)	
  DO_PERMUTATION(right, temp, left, 2, 0x33333333)	
  DO_PERMUTATION(right, temp, left, 8, 0x00ff00ff)
  right =  (right << 1) | (right >> 31);		
  temp  =  (left ^ right) & 0xaaaaaaaa;		
  right ^= temp;					
  left  ^= temp;					
  left  =  (left << 1) | (left >> 31);
}

/*
 * The 'inverse initial permutation'.
 */
#define FINAL_PERMUTATION(left, temp, right)		\
left  =  (left << 31) | (left >> 1);		\
temp  =  (left ^ right) & 0xaaaaaaaa;		\
left  ^= temp;					\
right ^= temp;					\
right  =  (right << 31) | (right >> 1);		\
DO_PERMUTATION(right, temp, left, 8, 0x00ff00ff)	\
DO_PERMUTATION(right, temp, left, 2, 0x33333333)	\
DO_PERMUTATION(left, temp, right, 16, 0x0000ffff)	\
DO_PERMUTATION(left, temp, right, 4, 0x0f0f0f0f)


/*
 * A full DES round including 'expansion function', 'sbox substitution'
 * and 'primitive function P' but without swapping the left and right word.
 * Please note: The data in 'from' and 'to' is already rotated one bit to
 * the left, done in the initial permutation.
 */
#define DES_ROUND(from, to, work, subkey) \
work = from ^ *subkey++;	\
to ^= pgm_dword(sbox8, work	& 0x3f );			\
to ^= pgm_dword(sbox6, (work>>8) & 0x3f );			\
to ^= pgm_dword(sbox4, (work>>16) & 0x3f );			\
to ^= pgm_dword(sbox2, (work>>24) & 0x3f );			\
work = ((from << 28) | (from >> 4)) ^ *subkey++;	\
to ^= pgm_dword(sbox7,  work	    & 0x3f );			\
to ^= pgm_dword(sbox5, (work>>8)  & 0x3f );			\
to ^= pgm_dword(sbox3, (work>>16) & 0x3f );			\
to ^= pgm_dword(sbox1, (work>>24) & 0x3f );

  static const unsigned long pgm_dword(const prog_uint32_t * box, int n) {
    return (unsigned long) pgm_read_dword(box+n);
  }

  static void read64bit(const byte data[], uint32_t & left, uint32_t &right) {
    left  = ((unsigned long)data[0] << 24) 
        | ((unsigned long)data[1] << 16) 
        | ((unsigned long)data[2] << 8) 
        | (unsigned long)data[3];
    right = ((unsigned long)data[4] << 24) 
        | ((unsigned long)data[5] << 16) 
        | ((unsigned long)data[6] << 8) 
        | (unsigned long)data[7];
  }
/*
#define READ_64BIT_DATA(data, left, right) \
  left  = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];  \
  right = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
*/
#define WRITE_64BIT_DATA(data, left, right) \
data[0] = (left >> 24) &0xff; data[1] = (left >> 16) &0xff; \
data[2] = (left >> 8) &0xff; data[3] = left &0xff;  \
data[4] = (right >> 24) &0xff; data[5] = (right >> 16) &0xff;  \
data[6] = (right >> 8) &0xff; data[7] = right &0xff;


  static void key_schedule (const byte * _rawkey, uint32_t * subkey); 
  unsigned long subkey(int i, boolean mode);
  unsigned long encrypt_subkey(int i);
  unsigned long decrypt_subkey(int i);

public:
  Des();
  Des(const byte * k);

  void key_set(const byte * key);
  void ecb_crypt(const char *, char *, int);
  void ecb_encrypt(const char * f, char * t) { return ecb_crypt(f,t,0); }
  void ecb_decrypt(const char * f, char * t) { return ecb_crypt(f,t,1); }

  void ecb_encrypt(char * buf, int n, char fill = 0x20) {
	  char t[8];
	  if ( n == 0 )
	  	  return;
	  for(int i = 0; i < n ; i += 8 ) {
		  memset(t, fill, 8);
		  memcpy(t, buf+i, n - i);
		  ecb_encrypt(t, buf+i);
	  }
  }

  void ecb_decrypt(char * buf, int n) {
	  char t[8];
	  if ( n > 0 ) {
		  for(int blk = 0; blk < (n>>3) ; blk++ ) {
			  ecb_decrypt(buf+(blk*8), t);
			  memcpy(t, buf+(blk*8), 8);
		  }
	  }
  }

};


#endif
