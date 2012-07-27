void log(){
 nowRmb.sec = (millis() - sensTimer)/100; //100m秒sensTimerなのはUpsenのたびに記録してるから。
 nowRmb.s = (nowRmb.s + logCount*5)%360;//今まで何度回転したか//int　シータ　現在の向き
 nowRmb.r = nowRmb.r + float(logCount*5*0.017);//float　ラジアン

 nowRmb.x = int(nowRmb.x + sin(nowRmb.r)*nowRmb.sec);
 nowRmb.y = int(nowRmb.y + cos(nowRmb.r)*nowRmb.sec);

 logCount = 0;
//goalとの差分などを出す予定。
//count = r/5/0.017
 //直進優先
int x = sin(nowRmb.r+90);
int y = cos(nowRmb.r+90);
 leaves.x = goal.x - nowRmb.x;
 leaves.y = goal.y - nowRmb.y;

 if((leaves.x>0 && x<=0)||(leaves.x<0 && x>0)||(leaves.y>0 && y<0)||(leaves.y<0 && y>0))//90°反時計回りで逆方向に
 	tr = 1;
 else
 	tr = 0;


}
