#include "Des.h"

Des coder;

void setup() {
  char buf[32];

  const byte key[8] = { 
    0x5C, 0x78, 0x51, 0xCB, 0x6A, 0xB5, 0x4E, 0x5E        };
  unsigned char plaintext[32];
  unsigned char ciphertext[32];

  int i;

  Serial.begin(9600);
  Serial.println("Start.");
  Serial.println();

  // Fill 'key' and 'plaintext' with some data
  strncpy((char *) plaintext, "Hello, World!\n", 16);

  Serial.println("Plain text: ");
  printCode(plaintext, 32);
  Serial.println();
  
  Serial.println("Key: ");
  printCode((byte *) key, 8);
  Serial.println();

  // Set up the DES encryption context
  coder.key_set(key);
  // Encrypt the plaintext
  coder.ecb_encrypt((char*) plaintext, (char*) ciphertext);

  Serial.println("Cipher text: ");
  printCode(ciphertext, 32);
  Serial.println();

  // Decrypt
  coder.ecb_decrypt((char*) ciphertext, (char*) plaintext);
  Serial.println("Plain text: ");
  printCode(plaintext, 32);
  Serial.println();

}

void loop() {
}

void printCode(byte * array, int n) {
    for (int i = 0; i < n; i++) {
    if ( isprint(array[i]) ) {
      Serial.print('\'');
      Serial.print((char) array[i]);
      Serial.print('\'');
    } 
    else {
      Serial.print('$');
      Serial.print(array[i]>>4, HEX);
      Serial.print(array[i]&0x0f, HEX);
    }
    Serial.print(' ');
  }
  Serial.println();
}


