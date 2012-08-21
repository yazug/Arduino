#include <SPI.h>

#include <sd-reader_config.h>
#include <sd_raw.h>


int print_disk_info();
int sample();
int readDisk();

byte incomingByte;
void printWelcome();
long int address;
byte tempBytes[2];

//MemCard sd(4);

void setup() {

  Serial.begin(9600);
  delay(1000);
  Serial.println("Doing...");

SPI.begin();
  printWelcome();
  if(!sd_raw_init())
  {
    Serial.println("MMC/SD initialization failed");
  }
  print_disk_info();
  
}



void loop()
{
  int i;

  if(Serial.available()>0)
  {
    incomingByte=Serial.read();

    switch(incomingByte)
    {
    case 114:
      readDisk();
      break;
    case 115:
      sample();
      break;
    default:
      break;
    }
  }
}

int sample()
{
  int i,j;
  int temp;
  byte low;
  byte high;
  byte inByte;

  Serial.println();
  Serial.println();
  Serial.println("Sampling..");
  for(i=0;i<500;i=i+2)
  {
    if(Serial.available()>0)
    {
      inByte=Serial.read();
      if(inByte==113) return 0;
    }
    temp=analogRead(0);
    Serial.print(temp,DEC);
    Serial.print(" ");

    //Convert int to 2 bytes
    low=temp&0xFF;
    high=temp>>8;
    // Serial.print(temp,DEC);
    //Serial.print(low,DEC);
    //Serial.print(high,DEC);

    tempBytes[0]=low;
    tempBytes[1]=high;
    long t = millis();

    if(!sd_raw_write(i,(byte*)&t,4))
    {
      Serial.print("Write error");
    }
    //sd.sync();
    delay(2000);

    Serial.println();

  }

  return 1;
}


int readDisk()
{
  byte low;
  byte high;
  byte info[4];
  int i;
  int  result;
  Serial.println();
  for(i=0;i<128;i=i+4)
  {
    sd_raw_read(i,info,4);

    Serial.print(i);
    Serial.print(" ");
    Serial.print(*(long*)info,DEC);
    Serial.println(" ");
  }

}

void printWelcome()
{
  Serial.println("------------------------");
  Serial.println("Data sampling system");
  Serial.println("send r to read disk");
  Serial.println("send s to start sampling");
  Serial.println("send q to stop sampling");
  Serial.println("Ready.....");
  Serial.println("-------------------------");
}


int print_disk_info()
{

  sd_raw_info disk_info;
  if(!sd_raw_get_info(&disk_info))
  {
    return 0;
  }


  Serial.println();
  Serial.print("rev:    ");
  Serial.print(disk_info.revision,HEX);
  Serial.println();
  Serial.print("serial: 0x");
  Serial.print(disk_info.serial,HEX);
  Serial.println();
  Serial.print("date:   ");
  Serial.print(disk_info.manufacturing_month,DEC);
  Serial.println();
  Serial.print(disk_info.manufacturing_year,DEC);
  Serial.println();
  Serial.print("size:   ");
  Serial.print(disk_info.capacity,DEC);
  Serial.println();
  Serial.print("copy:   ");
  Serial.print(disk_info.flag_copy,DEC);
  Serial.println();
  Serial.print("wr.pr.: ");
  Serial.print(disk_info.flag_write_protect_temp,DEC);
  Serial.print('/');
  Serial.print(disk_info.flag_write_protect,DEC);
  Serial.println();
  Serial.print("format: ");
  Serial.print(disk_info.format,DEC);
  Serial.println();
  Serial.print("free:   ");

  return 1;
}


