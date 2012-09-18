/* d3des.h -
 *
 *	Headers and defines for d3des.c
 *	Graven Imagery, 1992.
 *
 * Copyright (c) 1988,1989,1990,1991,1992 by Richard Outerbridge
 *	(GEnie : OUTER; CIS : [71755,204])
 *
 *	This code is modified by Sin to fit a C++ Arduino library code.
 *	Sep 18, 2012
 *
 */

#ifndef _D3DES_H_
#define _D3DES_H_

//#include <stdint.h>
#include <Arduino.h>

//#define D2_DES		/* include double-length support */
//#define D3_DES		/* include triple-length support */

#ifdef D3_DES
#ifndef D2_DES
#define D2_DES		/* D2_DES is needed for D3_DES */
#endif
#endif

/* MODE == encrypt */
#define EN0	0
/* MODE == decrypt */
#define DE1	1

class D3DES {
public:
	static const byte ENCODE = 0;
	/* MODE == encrypt */
	static const byte DECODE = 1;
	/* MODE == decrypt */

private:
	static unsigned long KnL[32];

#ifdef D2_DES
	static unsigned long KnR[32] = {0L};
	static unsigned long Kn3[32] = {0L};
	static unsigned char Df_Key[24] = {
		0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,
		0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10,
		0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67};
#endif

	static unsigned short bytebit[8];
	static unsigned long bigbyte[24];

	/* Use the key schedule specified in the Standard (ANSI X3.92-1981). */

	static unsigned char pc1[56];
	static unsigned char totrot[16];
	static unsigned char pc2[48];

	/* A useful alias on 68000-ish machines, but NOT USED HERE. */
	/*
	 typedef union {
	 unsigned long blok[2];
	 unsigned short word[4];
	 unsigned char byte[8];
	 } M68K;
	 */

	void desfunc(unsigned long *, const unsigned long *);
	void scrunch(const unsigned char *, unsigned long *);
	void unscrun(const unsigned long *, unsigned char *);
	void cookey(unsigned long *);

public:
	void deskey(const uint8_t *, const uint8_t);
	/*		      hexkey[8]     MODE
	 * Sets the internal key register according to the hexadecimal
	 * key contained in the 8 bytes of hexkey, according to the DES,
	 * for encryption or decryption according to MODE.
	 */

	void usekey(unsigned long *);
	/*		    cookedkey[32]
	 * Loads the internal key register with the data in cookedkey.
	 */

	void cpkey(unsigned long *);
	/*		   cookedkey[32]
	 * Copies the contents of the internal key register into the storage
	 * located at &cookedkey[0].
	 */

	void des(const uint8_t *, uint8_t *);
	/*		    from[8]	      to[8]
	 * Encrypts/Decrypts (according to the key currently loaded in the
	 * internal key register) one block of eight bytes at address 'from'
	 * into the block at address 'to'.  They can be the same.
	 */

	void des(const uint8_t *, uint8_t *, int );

#ifdef D2_DES

#define desDkey(a,b)	des2key((a),(b))
	extern void des2key(unsigned char *, short);
	/*		      hexkey[16]     MODE
	 * Sets the internal key registerS according to the hexadecimal
	 * keyS contained in the 16 bytes of hexkey, according to the DES,
	 * for DOUBLE encryption or decryption according to MODE.
	 * NOTE: this clobbers all three key registers!
	 */

	extern void Ddes(unsigned char *, unsigned char *);
	/*		    from[8]	      to[8]
	 * Encrypts/Decrypts (according to the keyS currently loaded in the
	 * internal key registerS) one block of eight bytes at address 'from'
	 * into the block at address 'to'.  They can be the same.
	 */

	extern void D2des(unsigned char *, unsigned char *);
	/*		    from[16]	      to[16]
	 * Encrypts/Decrypts (according to the keyS currently loaded in the
	 * internal key registerS) one block of SIXTEEN bytes at address 'from'
	 * into the block at address 'to'.  They can be the same.
	 */

	extern void makekey(char *, unsigned char *);
	/*		*password,	single-length key[8]
	 * With a double-length default key, this routine hashes a NULL-terminated
	 * string into an eight-byte random-looking key, suitable for use with the
	 * deskey() routine.
	 */

#define makeDkey(a,b)	make2key((a),(b))
	extern void make2key(char *, unsigned char *);
	/*		*password,	double-length key[16]
	 * With a double-length default key, this routine hashes a NULL-terminated
	 * string into a sixteen-byte random-looking key, suitable for use with the
	 * des2key() routine.
	 */

#ifndef D3_DES	/* D2_DES only */

#define useDkey(a)	use2key((a))
#define cpDkey(a)	cp2key((a))

	extern void use2key(unsigned long *);
	/*		    cookedkey[64]
	 * Loads the internal key registerS with the data in cookedkey.
	 * NOTE: this clobbers all three key registers!
	 */

	extern void cp2key(unsigned long *);
	/*		   cookedkey[64]
	 * Copies the contents of the internal key registerS into the storage
	 * located at &cookedkey[0].
	 */

#else	/* D3_DES too */

#define useDkey(a)	use3key((a))
#define cpDkey(a)	cp3key((a))

	extern void des3key(unsigned char *, short);
	/*		      hexkey[24]     MODE
	 * Sets the internal key registerS according to the hexadecimal
	 * keyS contained in the 24 bytes of hexkey, according to the DES,
	 * for DOUBLE encryption or decryption according to MODE.
	 */

	extern void use3key(unsigned long *);
	/*		    cookedkey[96]
	 * Loads the 3 internal key registerS with the data in cookedkey.
	 */

	extern void cp3key(unsigned long *);
	/*		   cookedkey[96]
	 * Copies the contents of the 3 internal key registerS into the storage
	 * located at &cookedkey[0].
	 */

	extern void make3key(char *, unsigned char *);
	/*		*password,	triple-length key[24]
	 * With a triple-length default key, this routine hashes a NULL-terminated
	 * string into a twenty-four-byte random-looking key, suitable for use with
	 * the des3key() routine.
	 */

#endif	/* D3_DES */
#endif	/* D2_DES */

};

#endif // _D3DES_H_
/* d3des.h V5.09 rwo 9208.04 15:06 Graven Imagery
 ********************************************************************/
