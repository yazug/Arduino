/// ルンバの状態をセンサー値によって変更する 毎回訪れる///
int MakeRSt(){//もっとスマートにしたい
	/////内積などで出したほうが良さげ　目的地に近付いた時
	if((leaves.x <= 10 && leaves.x >= -10) || (leaves.y <= 10 && leaves.y >= -10)){//これはループの最初がいいかも
		if(roomba != REACH )
			return REACH;
		else if(millis()-doTimer > leaves.s){//回った
			        count = count + leaves.s /100;
			  	    logCount = logCount + leaves.s/100;
		            doTimer = 0;
		            time = millis();
		            upSen();
		            sensTimer = millis();
		            return FORWARD;
				  }
		  sensTimer = millis();
	}
	//////
	else if(BUMPRIGHT == 1 || BUMPLEFT == 1 || cmt < 20) {//BUMP
	  if(doTimer == 0 || roomba == WAIT){
		  return BACK;
	  } else if(roomba == BACK){
		  return BUMP;
	  } else if((BUMPRIGHT == 1 && BUMPLEFT == 1)|| cmt < 20) { //正面
		  if(millis()-doTimer > BUMPCT){//90°回った
            doTimer = 0;
            time = millis();
            upSen();
            sensTimer = millis();
            return FORWARD;
		  }
	  } else { //左右衝突//////////////////
		  if(millis()-doTimer > BUMPLRT){//回転
		  doTimer = 0;
          time = millis();
          upSen();
          sensTimer = millis();
          return FORWARD;
        }
    }
	  sensTimer = millis();//
  } else if(roomba == WAIT){
	  return WAIT;
  } else if(cml < WALL || cm < WALL || roomba == NEAR){ //ものが
    if(roomba != NEAR){
    	  return NEAR;
	  }else if(millis()-doTimer > NEART){//約15°回転
        doTimer = 0;//@
        return WAIT;
      }
  } else if(roomba != FORWARD){
    return FORWARD;
  }
    return NOTHING;//前回と変化なし
  }
