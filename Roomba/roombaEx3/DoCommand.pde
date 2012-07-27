/// ステータスに合わせて動作 //////////////////
void DoCom(int newroomba){
  switch(newroomba) 
  {
  case FORWARD://前進
	      goForward();
    roomba = FORWARD;
    break;
  case BUMP://ぶつかったとき
	  if(millis() - doTimer > 100){//after back
		  if(BUMPRIGHT == 1 && BUMPLEFT == 1){//BUMPCENTER
			  if(tr==1){//右回転が都合よいとき
				  spinRight();
				  count = count - BUMPCT/100;
				  logCount = logCount - BUMPCT/100;
			  }
			  else{
				  spinLeft();
				  count = count + BUMPCT/100;
				  logCount = logCount + BUMPCT/100;
			  }
		  }else if(BUMPRIGHT == 1){//BUMPRIGHT
			  spinLeft();
			  count = count + BUMPLRT/100;
			  logCount = logCount + BUMPLRT/100;
		  }else{//BUMPLEFT
			  spinRight();
			  count = count - BUMPLRT/100;
			  logCount = logCount - BUMPLRT/100;
		  }
		  doTimer = millis();
		  roomba = BUMP;
	  }
    break;
  case NEAR://壁に近いとき
	  if(cml < WALL){
		  turnRight();
		  count--;
		  logCount--;
	  }else{
		  turnLeft();
		  count++;
		  logCount++;
	  }
    doTimer = millis();
    roomba = NEAR;
    break;    
  case WAIT: //連続で曲がるの防止　時間調整すべし
	  if(doTimer == 0){
		  goForward();
		  doTimer = millis();
		  roomba = WAIT;
	  }else if(millis()-doTimer > 1000){
		  roomba = NOTHING;
		  doTimer = 0;
	  }
	  break;
  case BACK: //衝突時少し下がる。
	    log();
	    goBackward();
	    doTimer = millis();
	    if((BUMPRIGHT == 1 && BUMPLEFT == 1)|| cmt < 20){
	    	dataString = "Center";
	    }else if(BUMPRIGHT == 1){
	    	dataString = "Right ";
	    }else{
	    	dataString = "Left  ";
	    }

	    saveSD();
	    sensTimer=millis();
	    roomba = BACK;
	    break;
  }
  case REACH://目的地に近付いた時
	  if((leaves.x <= 10 && leaves.x >= -10) && (leaves.y <= 10 && leaves.y >= -10)){//目的地についた
		  Rstop();
		  break;
	  }else if(leaves.x <= 10 && leaves.x >= -10){//xは目的地に着いた
		  if(leaves.y>0){
			  leaves.s = (360-nowRmb.s)/5*100;
		  }else{
			  leaves.s = (180-nowRmb.s)/5*100;
		  }
	  	}else{//ｙは目的地に It doesn't matter.
	  	 if(leaves.x>0){
	  		leaves.s = (360-nowRmb.s)/5*100;
	  	  }else{
	  		leaves.s = (180-nowRmb.s)/5*100;
	  				  }
	  	}
	  spinLeft();
		 doTimer = millis();
		  roomba = REACH;
	  break;
}
