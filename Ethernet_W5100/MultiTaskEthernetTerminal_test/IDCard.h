const byte KawazuKey_a[] = {
  0xAA, 0x4B, 0x61, 0x5A, 0x75, 0x49, 0x69 };
const byte KawazuKey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };

/*
 char kana[][4] = {
 "xx", "xx", "xx", "xx", "xx", "xx", "wo", "xa",
 "xi", "xu", "xe", "xo", "ya", "yu", "yo", "xtu",
 "-", "A", "I", "U", "E", "O", "KA", "KI",
 "KU", "KE", "KO", "SA", "SHI", "SU", "SE", "SO",
 "aaa", "bbb", "ccc", "ddd", "eee", "NA", "NI", "NU",
 "NE", "NO", "HA", "HI", "FU", "HE", "HO", "MA",
 "MI", "MU", "ME", "MO", "YA", "YU", "YO", "RA",
 "RI", "RU", "RE", "RO", "WA", "NN", "\"", "o",
 };
 */

union IDCardData {
  struct FCF {
    enum GENDER {
      UNKNOWN = 0, MALE = 1, FEMALE = 2
    };
    static const long KyushuInstOfTech = 40010071;
    word division;
    char pin[12];
    char issue;
    char gender;
    //
    char kana[16];
    char school[8];
    char dateofissue[8];
    char goodthru[8];
    byte pmcontrol[8];
  } 
  felica;
  struct IizukaMagTape {
    word division;
    char pin[8];
    char issue;
    char res0[5];
    word kanji[8];
    char dayofbirth[7];
    char gender;
    char dateofissue[7];
    char res1[1];
  } 
  mifare;
};




