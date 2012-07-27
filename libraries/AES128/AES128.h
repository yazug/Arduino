/*
 *
 * AES 128 encryption/decryption utility routines
 * based on the codes in an ATMEL's application note
 *
 */

#ifndef AES128_H
#define AES128_H

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class AES128 {
	const static byte Sbox[256];
	const static byte inv_Sbox[256];
	const static byte rcon [13];
	byte codekey[16];
	byte key[16];
	byte state[16];
public:	
	AES128(const byte * skey);
	
	byte * encrypt(byte *message);
	byte * decrypt(byte *message);
//	void setKey(byte *ukey);

private:
	void initKey();
	void addRoundKey();
	void inv_addRoundKey(int i);
	void subBytes();
	void inv_subBytes();
	void shiftRows();
	void inv_shiftRows();
	byte xtime(byte x);
	void mixColumns();
	void inv_mixColumns();
	void computeKey(byte rcon);
	void computeKeyRound(int round);	
};
#endif // AES128_H
