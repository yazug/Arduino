//#include "Des.h"
// 18.702 bytes.
#include "d3des.h"

//Des coder;
D3DES codec;

void setup() {
  char buf[32];
  
  const byte key[8] = { 
    0x5C, 0x78, 0x51, 0xCB, 0x6A, 0xB5, 0x4E, 0x5E        };
  unsigned char plaintext[32];
  unsigned char ciphertext[32];
  memset(ciphertext, 0, 32);
  memset(plaintext, 0, 32);
  strcpy((char*) plaintext, "Hello, World!\n");
  
  int i;

  delay(500);

  Serial.begin(9600);
  Serial.flush();

  Serial.println("\n\n\nStart.\n");

  Serial.println("Original text: ");
  printCode(plaintext, 32);
  Serial.println();
  
  Serial.println("Key: ");
  printCode((byte *) key, 8);
  Serial.println();

  // Set up the DES encryption context
  //coder.key_set(key);
  codec.deskey(key, D3DES::ENCODE);
  // Encrypt the plaintext
//  coder.ecb_encrypt((char*) plaintext, (char*) ciphertext);
  codec.des(plaintext, ciphertext, 24);
  
  Serial.println("Encrypted text: ");
  printCode(ciphertext, 32);
  Serial.println();

  // Decrypt
//  coder.ecb_decrypt((char*) ciphertext, (char*) plaintext);
  codec.deskey((byte*)key, D3DES::DECODE);
  memset(plaintext, 0, 32);
  codec.des(ciphertext, plaintext, 24);
  Serial.println("Decrypted text: ");
  printCode(plaintext, 32);
  Serial.println();

}

void loop() {
}

void printCode(byte * array, int n) {
    for (int i = 0; i < n; i++) {
    if ( isprint(array[i]) ) {
      //Serial.print('\'');
      Serial.print((char) array[i]);
      //Serial.print('\'');
    } 
    else {
      //Serial.print('$');
      Serial.print(array[i]>>4, HEX);
      Serial.print(array[i]&0x0f, HEX);
    }
    Serial.print(' ');
  }
  Serial.println();
}


