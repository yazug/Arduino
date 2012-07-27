
const int DIGIT[] = {
  6, 5, 3, 2, 4};
const int DIGITS = sizeof(DIGIT)/sizeof(int);
const int SEGMENT[] = {
  15, 11, 14, 7, 8, 10, 16, 9};
const int SEGMENTS = sizeof(SEGMENT)/sizeof(int);
int seg;
int period = 500;

void setup() {
  for (int d = 0; d < DIGITS; d++) {
    pinMode(DIGIT[d], OUTPUT);
    digitalWrite(DIGIT[d], LOW);
  }
  for (int s = 0; s < SEGMENTS; s++) {
    pinMode(SEGMENT[s], OUTPUT);
    digitalWrite(SEGMENT[s], HIGH);
  }

  seg = 0;
}

void loop() {
  seg = 0;
  digit(seg, millis()/100%10);
  delayMicroseconds(max(period, 400));
  digit(seg, ' ');
  seg = 1;
  digit(seg, millis()/1000%10);
  delayMicroseconds(max(period, 400));
  digit(seg, ' ');
  seg = 2;
  digit(seg, millis()/10000%10);
  delayMicroseconds(max(period, 400));
  digit(seg, ' ');
  seg = 3;
  digit(seg, millis()/100000%10);
  delayMicroseconds(max(period, 400));
  digit(seg, ' ');
  seg = 4;
  digit(seg, millis()/1000%2);
  delayMicroseconds(max(period, 400));
  digit(seg, ' ');
//  delayMicroseconds(max(0, period - 400));
  
}

const byte PATTERN[] = {
  0b00111111,
  0b00000110,
  0b01011011,
  0b01001111,
  0b01100110,
  0b01101101,
  0b01111101,
  0b00000111,
  0b01111111,
  0b01101111,
  0, // 10
  0b01110111,  // A
  0b01111001  // C
};

void digit(int d, char val) {
  int patNo;
  switch (val) {
  case 0:
  case '0':
    patNo = 0;
    break;
  case 1:
  case '1':
    patNo = 1;
    break;
  case 2:
  case '2':
    patNo = 2;
    break;
  case 3:
  case '3':
    patNo = 3;
    break;
  case 4:
  case '4':
    patNo = 4;
    break;
  case 5:
  case '5':
    patNo = 5;
    break;
  case 6:
  case '6':
    patNo = 6;
    break;
  case 7:
  case '7':
    patNo = 7;
    break;
  case 8:
  case '8':
    patNo = 8;
    break;
  case 9:
  case '9':
    patNo = 9;
    break;
  default:
    digitalWrite(DIGIT[d], LOW);
    for (int i = 0; i < 8; i++) {
      digitalWrite(SEGMENT[i], HIGH);
    }
    return;
  }
  digitalWrite(DIGIT[d], HIGH);
  for (int i = 0; i < 8; i++) {
    if ( PATTERN[patNo] & 1<<i ) {
      digitalWrite(SEGMENT[i], LOW);
    }
  }
  return;  
}



