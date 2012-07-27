#include <WProgram.h>
#include <Print.h>

struct Eyesight {
  byte right, left;
};

struct BloodPressure {
  byte systolic, diastolic, heartbeat;
};

struct UrineExamination {
  byte blood, protein, glucose;
};

struct Date64 {
  word year;
  byte month, day;

  void set(unsigned long val) {
	year = val/10000;
	month = val/100%100;
	day = val%100;
  }
  
  void set(word y, byte m, byte d) {
	year = y;
	month = m;
	day = d;
  }
};

typedef byte block16[16];

class MedicalCheckup {
public:
  byte uid_length;
  byte uid[10];
  union {
	block16 id_block; //[3];
	struct {
	  unsigned long id;  // unsigned 32 bit
	  byte school, course;
	  Date64 birthday;
	  byte reserved_0[6];
	  //byte reserved_id_b[16];
	  //byte reserved_id_c[16];
	};
  };

  union {
	block16 block[3];
	struct {
	  // block 0
	  Date64 checkdate;
	  byte sex;
	  byte ordinal;
	  byte reserved_1[2];
	  word height, weight;
	  Eyesight vision[2];
	  // block 1
	  BloodPressure bp[3];
	  byte reserved_2[7];
	  // block 2
	  byte reserved_3[3];
	  UrineExamination ur[3];
	  word xray_no;
	  byte reserved_4;
	  byte checksum;
	};
  };
  
public:
  MedicalCheckup() {
	clear();
  }
  
  void clear() {
	for (int j = 0; j < 16; j++) 
	  id_block[j] = 0;
	for (int i = 0; i < 3; i++) 
	  for (int j = 0; j < 16; j++) 
		block[i][j] = 0;
  }
  
  byte crc8() {
	byte xsum = 0;
	for (int j = 0; j < 16; j++) 
	  xsum ^= id_block[j];
	for (int i = 0; i < 47; i++) 
		xsum ^= block[i>>4][i&0x0f];
	return xsum;
  }
  
  boolean uid_equals(const byte other[]) {
	for (int i = 0; i < uid_length; i++) {
	  if ( uid[i] != other[i] )
		return false;
	}
	return true;
  }
  
  byte * dataBlock(int i) {
	return (byte *) block[i];
  }
  
  Print & printOn(Print & stream) {
	char buf[36];
    char urindex[][3] = {"-","+-","+1","+2","+3","+4","+5", "NA"};
	sprintf(buf,"ID: %d [School %d, Course %d]\n", id, school, course);
	stream.print(buf);
	sprintf(buf,"Birthday: %04d/%02d/%02d.\n", birthday.year, birthday.month, birthday.day);
	stream.print(buf);
	sprintf(buf,"Check date: %04d/%02d/%02d, ", checkdate.year, checkdate.month, checkdate.day);
	stream.print(buf);
	if ( sex == 1 )
	  stream.print("M\n");
	else if ( sex == 2 )
	  stream.print("F\n");
	else
	  stream.print("?\n");
	stream.print("Blood Pressure:\n");
    stream.print("  Syst. Diast. H. Beat\n");
	for(int i = 0; i < 3; i++) {
	  sprintf(buf, "%d) %03d,  %03d,  %03d\n", i+1, bp[0].systolic,bp[0].diastolic,bp[0].heartbeat);
	  stream.print(buf);
	}
	stream.print("Urine Examination:\n");
	for(int i = 0; i < 3; i++) {
	  sprintf(buf," %d) Bld: %s, Prt: %s, Glu: %s\n", i+1, urindex[min(ur[0].blood,7)], urindex[min(ur[0].protein,7)], urindex[min(ur[0].glucose,7)]);
	  stream.print(buf);
	}
	stream.print(xray_no);
	return stream;
  }
  
  /*
  static boolean valid_date(Date64 & date) {
    if ( date.year > 2100 or date.year < 1986 ) 
      return false;
    if ( date.month > 12 or date.month < 1 )
	  return false;
	if ( date.day > 31 or date.day < 1 )
	  return false;
	return true;
  }
  */
  /*
  void set_BPH(int i, BloodPressure &bp) {
    systolic[i] = bp.systolic;
    diastolic[i] = bp.diastolic;
    heartbeat[i] = bp.heartbeat;
  }
  
  void get_BPH(int i, BloodPressure &bp) {
    bp.systolic = systolic[i];
    bp.diastolic = diastolic[i];
    bp.heartbeat = heartbeat[i];
  }
   */

};
