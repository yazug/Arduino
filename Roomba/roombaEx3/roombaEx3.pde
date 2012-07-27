//液晶ディスプレイに状態を表示しつつ (超音波×3、x,y)
//定期的にセンサーの値をとりつつ 状態によって場合分けしながら 進みます。
//超音波は左右につけ、近づいたら離れる
//現在地の情報をとるのは、センサー更新のとき、ぶつかったとき
#include <LiquidCrystal.h>//lcd
#include <SoftwareSerial.h>//lcd
#include <SerLCD.h>//lcd

#include <Wire.h>
#include <DS1307.h>
#include <SD.h> //SD

SerLCD lcd(3);//lcd DIGITALポート番号
DS1307 rtc = DS1307();
String dataString = "";

int value, valuel, valuet, cm, cml, cmt, count, logCount=0, tr=0;
//lは左のセンサ tは前のセンサ 無印は右のセンサ countは移動中の角度計算用　trは左右判断
int ReT;

char sen[26];
unsigned long time = 0;
long clockval, sensTimer=0;
long doTimer = 0;//動作変更までの時間用
// Sensor Packet 1
#define BUMPRIGHT (sen[0] & 0x01)  
#define BUMPLEFT  ((sen[0] & (1<<1))>>1)
//回転時間や距離
#define BUMPLRT 500 //バンプ左右の回転時間 100=5°　,count = Time/100
#define BUMPCT 1800 //バンプ中央の回転時間
#define NEART 80 //壁に近づいた時の回転時間
#define WALL 60 //近いとする壁との距離

//enum RoombaStatus {//ルンバステータス 
  const int  NOTHING = 0;
  const int  FORWARD = 1;
  const int  BUMP = 2;
  const int  NEAR = 3;
  const int  WAIT = 4;
  const int  BACK = 5;
  const int  CLEAN = 6;
  const int  REACH = 7;
//};
int roomba = NOTHING;//前の状態
int gX, gY;
struct locate{
	unsigned long sec; //距離とか　今は時間
	int s; //角度θ
	float r; //角度ラジアン
	int x;
	int y;
} nowRmb = {//現在地
		0,0,0,0,0
},

////goal////////////////////////////現在は適当に決めています
goal = {
		0,0,0,-500,800
},
////leaves//////////////////////////ゴールとの差分用
leaves = {
		0,0,0,0,0
};
inline void dd_pulse(int d = 50){
  digitalWrite(4, LOW);
  digitalWrite(13, HIGH);
  delay(d);
  digitalWrite(4, HIGH);
  digitalWrite(13, LOW);
}


void setup() {
	pinMode(30, INPUT);//スイッチｵﾚﾝｼﾞ
	pinMode(32, INPUT);//イエロー
//白いボタンはArduinoのリセット
	digitalWrite(30, HIGH);//
	digitalWrite(32, HIGH);//

  pinMode(13, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  digitalWrite(13, LOW);

  Wire.begin();
  rtc.init();

  Serial.begin(57600);
  pinMode(53, OUTPUT); //SD

  if (!SD.begin(53)) {//SD////
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  File dataFile = SD.open("rlog3.txt", FILE_WRITE);
  dataFile.println("BUMP, msec, degree, TIME, CSens, RSens, LSens");
  //バンプセンサーの位置,直進したmsec,角度（時計回りが－）,リアルタイム,超音波前,右,左
  dataFile.close();

  dd_pulse();
  delay(2000);
  dd_pulse();
  delay(250);
  dd_pulse();
  delay(250);
  dd_pulse();
  delay(250);

  delay(2000);
  Serial1.begin(19200);//Roomba

  lcd.begin(9600);//lcd/////////
  lcd.home();
  
  lcd.print("Start!");
  delay(1000);
  lcd.clear();//lcd////////////
  
  Serial1.print((byte) 128); // start
  Serial1.print((byte) 130); // control

  count = 0;
  //log(); //ここに入れるとｙに変な値がはいるcosθだから
  upSen();

  dataString= "START ";
  saveSD();
  time = millis();
  sensTimer = millis();
}

//// loop /////////////////////////////////////////////////////////////
void loop(){
  int newroomba;
  if(!digitalRead(30)||!digitalRead(32)){//ボタンが押された時 要改善
	  lcd.clear();//lcd////////////
	  delay(1000);//これを入れないと止まらない500でもいけるかも
	  Clean();
  }
  if(roomba != CLEAN){//移動中
	  if(millis() > sensTimer + 500){
		  log();
		  upSen();
		  sensTimer = millis();
	  }
	  newroomba = MakeRSt();//newroombaが送られる
	  if(newroomba != NOTHING){
		  DoCom(newroomba); //コマンド実行
	  }
  }
	  lcdPrint();

}
