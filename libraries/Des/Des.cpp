#include <avr/pgmspace.h>
#include "Des.h"

/*
 * The s-box values are permuted according to the 'primitive function P'
 * and are rotated one bit to the left.
 */
PROGMEM const prog_uint32_t Des::sbox1[64] = {
	0x01010400, 0x00000000, 0x00010000, 0x01010404, 0x01010004, 0x00010404,
	0x00000004, 0x00010000, 0x00000400, 0x01010400, 0x01010404, 0x00000400,
	0x01000404, 0x01010004, 0x01000000, 0x00000004, 0x00000404, 0x01000400,
	0x01000400, 0x00010400, 0x00010400, 0x01010000, 0x01010000, 0x01000404,
	0x00010004, 0x01000004, 0x01000004, 0x00010004, 0x00000000, 0x00000404,
	0x00010404, 0x01000000, 0x00010000, 0x01010404, 0x00000004, 0x01010000,
	0x01010400, 0x01000000, 0x01000000, 0x00000400, 0x01010004, 0x00010000,
	0x00010400, 0x01000004, 0x00000400, 0x00000004, 0x01000404, 0x00010404,
	0x01010404, 0x00010004, 0x01010000, 0x01000404, 0x01000004, 0x00000404,
	0x00010404, 0x01010400, 0x00000404, 0x01000400, 0x01000400, 0x00000000,
	0x00010004, 0x00010400, 0x00000000, 0x01010004
};

PROGMEM const prog_uint32_t Des::sbox2[64] = {
	0x80108020, 0x80008000, 0x00008000, 0x00108020, 0x00100000, 0x00000020,
	0x80100020, 0x80008020, 0x80000020, 0x80108020, 0x80108000, 0x80000000,
	0x80008000, 0x00100000, 0x00000020, 0x80100020, 0x00108000, 0x00100020,
	0x80008020, 0x00000000, 0x80000000, 0x00008000, 0x00108020, 0x80100000,
	0x00100020, 0x80000020, 0x00000000, 0x00108000, 0x00008020, 0x80108000,
	0x80100000, 0x00008020, 0x00000000, 0x00108020, 0x80100020, 0x00100000,
	0x80008020, 0x80100000, 0x80108000, 0x00008000, 0x80100000, 0x80008000,
	0x00000020, 0x80108020, 0x00108020, 0x00000020, 0x00008000, 0x80000000,
	0x00008020, 0x80108000, 0x00100000, 0x80000020, 0x00100020, 0x80008020,
	0x80000020, 0x00100020, 0x00108000, 0x00000000, 0x80008000, 0x00008020,
	0x80000000, 0x80100020, 0x80108020, 0x00108000
};

PROGMEM const prog_uint32_t Des::sbox3[64] = {
	0x00000208, 0x08020200, 0x00000000, 0x08020008, 0x08000200, 0x00000000,
	0x00020208, 0x08000200, 0x00020008, 0x08000008, 0x08000008, 0x00020000,
	0x08020208, 0x00020008, 0x08020000, 0x00000208, 0x08000000, 0x00000008,
	0x08020200, 0x00000200, 0x00020200, 0x08020000, 0x08020008, 0x00020208,
	0x08000208, 0x00020200, 0x00020000, 0x08000208, 0x00000008, 0x08020208,
	0x00000200, 0x08000000, 0x08020200, 0x08000000, 0x00020008, 0x00000208,
	0x00020000, 0x08020200, 0x08000200, 0x00000000, 0x00000200, 0x00020008,
	0x08020208, 0x08000200, 0x08000008, 0x00000200, 0x00000000, 0x08020008,
	0x08000208, 0x00020000, 0x08000000, 0x08020208, 0x00000008, 0x00020208,
	0x00020200, 0x08000008, 0x08020000, 0x08000208, 0x00000208, 0x08020000,
	0x00020208, 0x00000008, 0x08020008, 0x00020200
};

PROGMEM const prog_uint32_t Des::sbox4[64] = {
	0x00802001, 0x00002081, 0x00002081, 0x00000080, 0x00802080, 0x00800081,
	0x00800001, 0x00002001, 0x00000000, 0x00802000, 0x00802000, 0x00802081,
	0x00000081, 0x00000000, 0x00800080, 0x00800001, 0x00000001, 0x00002000,
	0x00800000, 0x00802001, 0x00000080, 0x00800000, 0x00002001, 0x00002080,
	0x00800081, 0x00000001, 0x00002080, 0x00800080, 0x00002000, 0x00802080,
	0x00802081, 0x00000081, 0x00800080, 0x00800001, 0x00802000, 0x00802081,
	0x00000081, 0x00000000, 0x00000000, 0x00802000, 0x00002080, 0x00800080,
	0x00800081, 0x00000001, 0x00802001, 0x00002081, 0x00002081, 0x00000080,
	0x00802081, 0x00000081, 0x00000001, 0x00002000, 0x00800001, 0x00002001,
	0x00802080, 0x00800081, 0x00002001, 0x00002080, 0x00800000, 0x00802001,
	0x00000080, 0x00800000, 0x00002000, 0x00802080
};

PROGMEM const prog_uint32_t Des::sbox5[64] = {
	0x00000100, 0x02080100, 0x02080000, 0x42000100, 0x00080000, 0x00000100,
	0x40000000, 0x02080000, 0x40080100, 0x00080000, 0x02000100, 0x40080100,
	0x42000100, 0x42080000, 0x00080100, 0x40000000, 0x02000000, 0x40080000,
	0x40080000, 0x00000000, 0x40000100, 0x42080100, 0x42080100, 0x02000100,
	0x42080000, 0x40000100, 0x00000000, 0x42000000, 0x02080100, 0x02000000,
	0x42000000, 0x00080100, 0x00080000, 0x42000100, 0x00000100, 0x02000000,
	0x40000000, 0x02080000, 0x42000100, 0x40080100, 0x02000100, 0x40000000,
	0x42080000, 0x02080100, 0x40080100, 0x00000100, 0x02000000, 0x42080000,
	0x42080100, 0x00080100, 0x42000000, 0x42080100, 0x02080000, 0x00000000,
	0x40080000, 0x42000000, 0x00080100, 0x02000100, 0x40000100, 0x00080000,
	0x00000000, 0x40080000, 0x02080100, 0x40000100
};

PROGMEM const prog_uint32_t Des::sbox6[64] = {
	0x20000010, 0x20400000, 0x00004000, 0x20404010, 0x20400000, 0x00000010,
	0x20404010, 0x00400000, 0x20004000, 0x00404010, 0x00400000, 0x20000010,
	0x00400010, 0x20004000, 0x20000000, 0x00004010, 0x00000000, 0x00400010,
	0x20004010, 0x00004000, 0x00404000, 0x20004010, 0x00000010, 0x20400010,
	0x20400010, 0x00000000, 0x00404010, 0x20404000, 0x00004010, 0x00404000,
	0x20404000, 0x20000000, 0x20004000, 0x00000010, 0x20400010, 0x00404000,
	0x20404010, 0x00400000, 0x00004010, 0x20000010, 0x00400000, 0x20004000,
	0x20000000, 0x00004010, 0x20000010, 0x20404010, 0x00404000, 0x20400000,
	0x00404010, 0x20404000, 0x00000000, 0x20400010, 0x00000010, 0x00004000,
	0x20400000, 0x00404010, 0x00004000, 0x00400010, 0x20004010, 0x00000000,
	0x20404000, 0x20000000, 0x00400010, 0x20004010
};

PROGMEM const prog_uint32_t Des::sbox7[64] = {
	0x00200000, 0x04200002, 0x04000802, 0x00000000, 0x00000800, 0x04000802,
	0x00200802, 0x04200800, 0x04200802, 0x00200000, 0x00000000, 0x04000002,
	0x00000002, 0x04000000, 0x04200002, 0x00000802, 0x04000800, 0x00200802,
	0x00200002, 0x04000800, 0x04000002, 0x04200000, 0x04200800, 0x00200002,
	0x04200000, 0x00000800, 0x00000802, 0x04200802, 0x00200800, 0x00000002,
	0x04000000, 0x00200800, 0x04000000, 0x00200800, 0x00200000, 0x04000802,
	0x04000802, 0x04200002, 0x04200002, 0x00000002, 0x00200002, 0x04000000,
	0x04000800, 0x00200000, 0x04200800, 0x00000802, 0x00200802, 0x04200800,
	0x00000802, 0x04000002, 0x04200802, 0x04200000, 0x00200800, 0x00000000,
	0x00000002, 0x04200802, 0x00000000, 0x00200802, 0x04200000, 0x00000800,
	0x04000002, 0x04000800, 0x00000800, 0x00200002
};

PROGMEM const prog_uint32_t Des::sbox8[64] = {
	0x10001040, 0x00001000, 0x00040000, 0x10041040, 0x10000000, 0x10001040,
	0x00000040, 0x10000000, 0x00040040, 0x10040000, 0x10041040, 0x00041000,
	0x10041000, 0x00041040, 0x00001000, 0x00000040, 0x10040000, 0x10000040,
	0x10001000, 0x00001040, 0x00041000, 0x00040040, 0x10040040, 0x10041000,
	0x00001040, 0x00000000, 0x00000000, 0x10040040, 0x10000040, 0x10001000,
	0x00041040, 0x00040000, 0x00041040, 0x00040000, 0x10041000, 0x00001000,
	0x00000040, 0x10040040, 0x00001000, 0x00041040, 0x10001000, 0x00000040,
	0x10000040, 0x10040000, 0x10040040, 0x10000000, 0x00040000, 0x10001040,
	0x00000000, 0x10041040, 0x00040040, 0x10000040, 0x10040000, 0x10001000,
	0x10001040, 0x00000000, 0x10041040, 0x00041000, 0x00041000, 0x00001040,
	0x00001040, 0x00040040, 0x10000000, 0x10041000
};

PROGMEM const prog_uint32_t Des::leftkey_swap[16] = {
  0x00000000, 0x00000001, 0x00000100, 0x00000101,
  0x00010000, 0x00010001, 0x00010100, 0x00010101,
  0x01000000, 0x01000001, 0x01000100, 0x01000101,
  0x01010000, 0x01010001, 0x01010100, 0x01010101
};

PROGMEM const prog_uint32_t Des::rightkey_swap[16] = {
  0x00000000, 0x01000000, 0x00010000, 0x01010000,
  0x00000100, 0x01000100, 0x00010100, 0x01010100,
  0x00000001, 0x01000001, 0x00010001, 0x01010001,
  0x00000101, 0x01000101, 0x00010101, 0x01010101,
};

const byte Des::encrypt_rotate_tab[16] = {
  1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

Des::Des() {
}

Des::Des(const byte * key) {
	key_set(key);
}

void Des::key_schedule (const byte * _rawkey, uint32_t * subkey)  {
  const unsigned char *rawkey = (const unsigned char *) _rawkey;
  uint32_t left, right, work;
  int round;

/*
  READ_64BIT_DATA (rawkey, left, right)
  Serial.print("\nleft: ");
  Serial.println((unsigned long)left, HEX);
  Serial.print("right: ");
  Serial.println(right, HEX);
  */
  read64bit(rawkey, left, right);  
  DO_PERMUTATION (right, work, left, 4, 0x0f0f0f0f)
  DO_PERMUTATION (right, work, left, 0, 0x10101010)
  left = ((pgm_dword(leftkey_swap, (left >> 0) & 0xf) << 3)
    | (pgm_dword(leftkey_swap,(left >> 8) & 0xf) << 2)
    | (pgm_dword(leftkey_swap,(left >> 16) & 0xf) << 1)
    | (pgm_dword(leftkey_swap,(left >> 24) & 0xf))
    | (pgm_dword(leftkey_swap,(left >> 5) & 0xf) << 7)
    | (pgm_dword(leftkey_swap,(left >> 13) & 0xf) << 6)
    | (pgm_dword(leftkey_swap,(left >> 21) & 0xf) << 5)
    | (pgm_dword(leftkey_swap,(left >> 29) & 0xf) << 4));

//  Serial.print("\nleft: ");
//  Serial.println((unsigned long)left, HEX);

  left &= 0x0fffffff;

  right = ((pgm_dword(rightkey_swap,(right >> 1) & 0xf) << 3)
    | (pgm_dword(rightkey_swap,(right >> 9) & 0xf) << 2)
    | (pgm_dword(rightkey_swap,(right >> 17) & 0xf) << 1)
    | (pgm_dword(rightkey_swap,(right >> 25) & 0xf) )
    | (pgm_dword(rightkey_swap,(right >> 4) & 0xf) << 7)
    | (pgm_dword(rightkey_swap,(right >> 12) & 0xf) << 6)
    | (pgm_dword(rightkey_swap,(right >> 20) & 0xf) << 5)
    | (pgm_dword(rightkey_swap,(right >> 28) & 0xf) << 4));

//  Serial.print("right: ");
//  Serial.println(right, HEX);

  right &= 0x0fffffff;

  for (round = 0; round < 16; ++round)    {

    left = ((left << encrypt_rotate_tab[round])
      | (left >> (28 - encrypt_rotate_tab[round]))) & 0x0fffffff;

    right = ((right << encrypt_rotate_tab[round])
      | (right >> (28 - encrypt_rotate_tab[round]))) & 0x0fffffff;

    *subkey++ = (((left << 4) & 0x24000000)
      | ((left << 28) & 0x10000000)
      | ((left << 14) & 0x08000000)
      | ((left << 18) & 0x02080000)
      | ((left << 6) & 0x01000000)
      | ((left << 9) & 0x00200000)
      | ((left >> 1) & 0x00100000)
      | ((left << 10) & 0x00040000)
      | ((left << 2) & 0x00020000)
      | ((left >> 10) & 0x00010000)
      | ((right >> 13) & 0x00002000)
      | ((right >> 4) & 0x00001000)
      | ((right << 6) & 0x00000800)
      | ((right >> 1) & 0x00000400)
      | ((right >> 14) & 0x00000200)
      | (right & 0x00000100)
      | ((right >> 5) & 0x00000020)
      | ((right >> 10) & 0x00000010)
      | ((right >> 3) & 0x00000008)
      | ((right >> 18) & 0x00000004)
      | ((right >> 26) & 0x00000002)
      | ((right >> 24) & 0x00000001));

    *subkey++ = (((left << 15) & 0x20000000)
      | ((left << 17) & 0x10000000)
      | ((left << 10) & 0x08000000)
      | ((left << 22) & 0x04000000)
      | ((left >> 2) & 0x02000000)
      | ((left << 1) & 0x01000000)
      | ((left << 16) & 0x00200000)
      | ((left << 11) & 0x00100000)
      | ((left << 3) & 0x00080000)
      | ((left >> 6) & 0x00040000)
      | ((left << 15) & 0x00020000)
      | ((left >> 4) & 0x00010000)
      | ((right >> 2) & 0x00002000)
      | ((right << 8) & 0x00001000)
      | ((right >> 14) & 0x00000808)
      | ((right >> 9) & 0x00000400)
      | ((right) & 0x00000200)
      | ((right << 7) & 0x00000100)
      | ((right >> 7) & 0x00000020)
      | ((right >> 3) & 0x00000011)
      | ((right << 2) & 0x00000004)
      | ((right >> 21) & 0x00000002));
  }

}

void Des::key_set(const byte * key) {
  int i;

  key_schedule (key, encrypt_subkeys);
  for (i = 0; i < 32; i += 2)    {
    decrypt_subkeys[i] = encrypt_subkeys[30 - i];
    decrypt_subkeys[i + 1] = encrypt_subkeys[31 - i];
  }
}

unsigned long Des::subkey(int i, boolean mode) {
	if ( mode ) {
		return decrypt_subkeys[i];
	} else {
		return encrypt_subkeys[i];
	}
}
/*
unsigned long Des::encrypt_subkey(int i) {
    return encrypt_subkeys[i];
}

unsigned long Des::decrypt_subkey(int i) {
    return encrypt_subkeys[30 - ((i>>1)<<1) + (i&1)];
}
*/
void Des::ecb_crypt(const char * _from, char * _to, int mode) {
	const unsigned char *from = (const unsigned char *) _from;
	unsigned char *to = (unsigned char *) _to;
	uint32_t left, right, work;
	uint32_t *keys;

  keys = mode ? decrypt_subkeys : encrypt_subkeys;
  read64bit(from, left, right);
//  INITIAL_PERMUTATION (left, work, right)
  initial_permutation(left,work, right);
 /*
  Serial.println("INITIAL_PERNUMTATION:");
  Serial.println(left, HEX);
  Serial.println(work, HEX);
  Serial.println(right, HEX);
  Serial.println(*keys, HEX);
*/
    DES_ROUND (right, left, work, keys) 
/*
      Serial.println("\nDES_ROUND:");
  Serial.println(left, HEX);
  Serial.println(right, HEX);
  Serial.println(work, HEX);
*/
    DES_ROUND (left, right, work, keys)
/*
      Serial.println("\nDES_ROUND:");
  Serial.println(left, HEX);
  Serial.println(right, HEX);
  Serial.println(work, HEX);
*/
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    FINAL_PERMUTATION (right, work, left)
    WRITE_64BIT_DATA (to, right, left)
}

