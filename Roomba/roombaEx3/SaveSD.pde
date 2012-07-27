void saveSD(){//SD
  long temp = 0;
  byte  buf[32];  



  dataString += ", ";
  dataString += (millis() - time);
  dataString += ", ";
  dataString += String(count*5);
  count = 0;
  rtc.getTime(temp);
  if ( temp != clockval ) {
    clockval = temp;
    sprintf((char*)buf, "%02lx:%02lx:%02lx ", clockval>>16&0xff, clockval>>8&0xff, clockval&0xff);
  }
  dataString += ", ";
  dataString += String((char*)buf);
  dataString += ", ";
  dataString += String(cmt);
  dataString += ", ";
  dataString += String(cm);
  dataString += ", ";
  dataString += String(cml);
  File dataFile = SD.open("rlog3.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  } 
}

