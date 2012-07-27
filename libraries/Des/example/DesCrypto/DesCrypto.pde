#include "Des.h"

Des coder;

void setup() {
  char buf[32];

  const byte key[8] = { 
    0x5C, 0x78, 0x51, 0xCB, 0x6A, 0xB5, 0x4E, 0x5E      };
  unsigned char plaintext[8];
  unsigned char ciphertext[8];

  int i;

  Serial.begin(9600);
  Serial.println();
  
  // Fill 'key' and 'plaintext' with some data
  strncpy((char *) plaintext, "Hello, World!\n", 8);
  for (i = 0; i < 8; i++) {
    Serial.print(plaintext[i]);
  }
  Serial.println();
  Serial.println();
   for (i = 0; i < 8; i++) {
     Serial.print(key[i]>>4&0x0f, HEX);
     Serial.print(key[i]&0x0f, HEX);
   }
   Serial.println();
   Serial.println();
  
  // Set up the DES encryption context
   coder.key_set((const char*) key);
   /*
   for (i = 0; i < 32; ) {
     Serial.print(coder.subkey(i++, true), HEX );
     Serial.print(" ");
     Serial.println(coder.subkey(i++, true), HEX );
   }
   Serial.println();
   */
  // Encrypt the plaintext
  coder.ecb_encrypt((char*) plaintext, (char*) ciphertext);
	
  for (i = 0; i < 8; i++) {
    Serial.print(ciphertext[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
  for (i = 0; i < 8; i++) {
    Serial.print((char)ciphertext[i]);
  }
  Serial.println();
  
  // Decrypt
  coder.ecb_decrypt((char*) ciphertext, (char*) plaintext);
  Serial.println();
  for (i = 0; i < 8; i++) {
    Serial.print((char) plaintext[i]);
  }
  Serial.println();

}

void loop() {
}


