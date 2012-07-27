
void lcdPrint(){
	if(roomba == CLEAN){
		lcd.setCursor(0, 0);
		lcd.print("CLEANING");
	}else{
  ////lcd////
  lcd.setCursor(0, 0);
  lcd.print("(");
  lcd.print(nowRmb.x);
  lcd.print(", ");
  lcd.print(nowRmb.y);
  lcd.print(") ");
  lcd.print(nowRmb.s);
  lcd.print("  ");
  lcd.setCursor(0, 1);
  lcd.print("F");
  lcd.print(cmt); 
  lcd.print(" ");
  lcd.setCursor(5, 1);
  lcd.print("L");
  lcd.print(cml);
  lcd.print(" ");
  lcd.setCursor(10, 1);
  lcd.print("R");
  lcd.print(cm);
  lcd.print(" ");
	}
}
