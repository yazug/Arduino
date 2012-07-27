// miss 

int ledPin = 13;

#include <SoftwareSerial.h>

inline void dd_pulse(int d = 50){
  digitalWrite(4, LOW);
  digitalWrite(13, HIGH);
  delay(d);
  digitalWrite(4, HIGH);
  digitalWrite(13, LOW);
}

SoftwareSerial serport(2,3);

void setup() {
  pinMode(13, OUTPUT);
  pinMode(2, INPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  digitalWrite(13, LOW);


  Serial.begin(57600);


  dd_pulse();
  delay(2000);
  dd_pulse();
  delay(250);
  dd_pulse();
  delay(250);
  dd_pulse();
  delay(250);

  delay(2000);

  serport.begin(19200);


  serport.print((byte) 128); // start
  serport.print((byte) 130); // control
  //Marioのテーマ
  serport.print((byte) 140);//song
  serport.print((byte) 1);//曲の番号
  serport.print((byte) 11);//音の数15まで

  serport.print((byte) 76);//音
  serport.print((byte) 16);  //長さ 
  serport.print((byte) 76);
  serport.print((byte) 16);
  serport.print((byte) 0);    
  serport.print((byte) 16); 
  serport.print((byte) 76);
  serport.print((byte) 16);
  serport.print((byte) 0);    
  serport.print((byte) 16); 
  serport.print((byte) 72);
  serport.print((byte) 16);
  serport.print((byte) 76);    
  serport.print((byte) 32); 
  serport.print((byte) 79);
  serport.print((byte) 32);
  serport.print((byte) 0);    
  serport.print((byte) 32); 
  serport.print((byte) 67);
  serport.print((byte) 32);
  serport.print((byte) 0);    
  serport.print((byte) 32); 

  Serial.println(" Song ");     
  serport.print((byte) 141);//pray 
  serport.print((byte) 1);//
  delay(2000);

}

void loop(){
  int c, cg, vl, cr, h = -1;
  char sen[10];
  char i = 0;
/*
  serport.print((byte) 137);  // drive
  serport.print((byte) 0x00); // speed
  serport.print((byte) 0xc8); // 
  serport.print((byte) 0x00); // 回転
  serport.print((byte) 0x01); // 
  delay(100);
  serport.print((byte) 139); //LED  
  serport.print((byte) 0x04); //DockC 
  serport.print((byte) 0); //color Green
  serport.print((byte) 255); //LightPower   
  delay(2000);


  //センサー読み取りコマンド   
  serport.print((byte) 142);  
  serport.print((byte) 3); //

  delay(100); // wait for sensors 

  while (serport.available()) { 
    c = serport.read();
    sen[i++] = c;
  }

  serport.print((byte) 139); //LED  
  serport.print((byte) 0x01); // DIRT
  serport.print((byte) 128); //color Orenge
  serport.print((byte) 255); //LightPower   
  delay(2000);

  vl =(sen[1] << 8) + sen[2];// mV  Voltage
  cg =(sen[6] << 8) + sen[7];// mAh Charge
  cr =(sen[3] << 8) + sen[4];// mA  Current 充電中に+、放電時に-
  //mAhとは、～mAを1時間流せるという意味
  h = (cg/cr)*(-1); //残り時間はcg÷cr、となる。

  if(cr<0){
    Serial.print("  Limit: "); 
    Serial.println(h);
  }
  else{
    Serial.println(" Charging "); 

  }  

  serport.print((byte) 139); //LED  
  serport.print((byte) 0x02); // SPOT
  serport.print((byte) 0); //color 
  serport.print((byte) 225); //LightPower   
  delay(2000);


  if(h <= 8 && h != -1){   
    Serial.println(" Go Dock! ");   
    serport.print((byte) 143);  
    delay(50000);
  }

  delay(100);
  serport.print((byte) 139); //LED  
  serport.print((byte) 0x08); // RoombaM
  serport.print((byte) 128); //color 
  serport.print((byte) 255); //LightPower   
  delay(2000);

  serport.print((byte) 137); // drive
  serport.print((byte) 0x00); // speed
  serport.print((byte) 0xc8); // 200
  serport.print((byte) 0x80); //前進   
  serport.print((byte) 0x00); // 
  delay(500);

  serport.print((byte) 137); // drive
  serport.print((byte) 0xff); // speed
  serport.print((byte) 0x38); // back -200
  serport.print((byte) 0x80); // 
  serport.print((byte) 0x00); // 
  delay(500); 

  serport.print((byte) 137); // drive
  serport.print((byte) 0x00); // speed
  serport.print((byte) 0x00); // stop
  serport.print((byte) 0x80); // 
  serport.print((byte) 0x00); // 
  delay(500); 
*/
}


