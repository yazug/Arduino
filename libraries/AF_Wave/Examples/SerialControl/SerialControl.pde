#include <AF_Wave.h>
#include <avr/pgmspace.h>
#include "util.h"
#include "wave.h"

#define HEYJUDE "HEYJUDE.WAV"
#define ONEKHZ "1KHZ.WAV"
#define GLASS "GLASWORK.WAV"
#define PENTHOUSE "PENTHOUS.WAV"

AF_Wave card;
File f;
Wavefile wave;      // only one!

#define redled 9

int wasplaying = 0;

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Wave test!");

  pinMode(2, OUTPUT); 
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(redled, OUTPUT);
  
  if (!card.init_card()) {
    putstring_nl("Card init. failed!"); return;
  }
  if (!card.open_partition()) {
    putstring_nl("No partition!"); return;
  }
  if (!card.open_filesys()) {
    putstring_nl("Couldn't open filesys"); return;
  }
  if (!card.open_rootdir()) {
    putstring_nl("Couldn't open dir"); return;
  }

  ls();
//  playfile(ONEKHZ);

}

void loop() { 
   char c, *toplay;
   
   if (Serial.available()) {
     c = Serial.read();
 Serial.println(c, BYTE);
     if (c == 'j') {
       toplay = HEYJUDE;
     }
     else if (c == 'g') {
       toplay = GLASS;
     }
     else if (c == 'p') {
       toplay = PENTHOUSE;
     }
     else if (c == '1') {
       toplay = ONEKHZ;
     } else {
       return;
     }
     
     if (wave.isplaying) {// already playing something, so stop it!
         wave.stop(); // stop it
         card.close_file(f);
     }
     playfile(toplay);
   }

   if (wave.isplaying && !wasplaying) {
     digitalWrite(redled, HIGH);
   } else if (!wave.isplaying && wasplaying) {
     digitalWrite(redled, LOW);
   }
   wasplaying = wave.isplaying;
}


void ls() {
  char name[13];
  card.reset_dir();
  putstring_nl("Files found:");
  while (1) {
    if (!card.get_next_name_in_dir(name)) {
       card.reset_dir();
       return;
    }
    Serial.println(name);
  }
}

void playfile(char *name) {
   f = card.open_file(name);
   if (!f) {
      putstring_nl("Couldn't open file"); return;
   }
   if (!wave.create(f)) {
     putstring_nl("Not a valid WAV"); return;
   }
   // ok time to play!
   wave.play();
}
