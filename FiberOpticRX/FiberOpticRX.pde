/* Arduino Fiber Optics Example Code
 * Copyright 2010 SparkFun Electronics
 * Written by Hilary Hoops
 * This code is example code for receiving a fiber optic signal at over 0.1Mbps
 * The digital input pin begins when it notices a change on the line (interrupt driven)
 * Then the controller reads in the data quickly then decodes it into its bi-phase form
 * then decodes it again into its binary form, finally printing out the received
 * character to the hardware serial port
 */

const int in = 4;           // Fiber Optic Rx pin
const int rawsize=500;      // Raw Data Size
const int BPDsize=150;      // Decoded Biphase Size
const int BDsize=150;       // Binary Data Size
int RawData[rawsize];       // Raw Data 
int BPdata[BPDsize];        // Biphase data
int Bdata[BDsize];          // Binary data 
char mychar;                // Character Read 

void setup()
{
  //Begin serial port for reporting character recieved
  Serial.begin(9600); 
  //Set input pin to input
  pinMode(in,INPUT); 
  //Start reading the data when a change happens on pin 3 (connect input pin and pin 3 together)
  attachInterrupt(1, my_interrupt_handler, CHANGE); 
}
void loop(){

}

void my_interrupt_handler() // This is essentially a "debounce" for the interrupt
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
  {
    FiberRead();
  }
  last_interrupt_time = interrupt_time;
} 

void FiberRead()
{ 
  //Collect all raw data coming in
  for(int i=0;i<rawsize;i++){
    RawData[i]=(PIND & B00010000)>>4;
  }
  //Raw data is read faster than it is transmitted, so we correct for timing and noise  
  int k=0;
  int q=0;
  for(int j=0;j<rawsize;j++){
    k++;
    //Serial.print(RawData[j]);  //This is for debugging it writes all the raw data
    if(RawData[j]!=RawData[j+1]){
      //Serial.println();       // Also for debugging, separates raw data into 1/0 clusters
      BPdata[q]=RawData[j];
      if(k>=9){
        q++;
        BPdata[q]=RawData[j];
      }
      q++;
      k=0;   
    }
  }

  //Decode the Biphase data into Binary
  int m=0;
  int j=0;
  while(BPdata[m]!=BPdata[m+1])
    m++;  
  for(m; m<BPDsize;m=m+2){
    if(BPdata[m]==BPdata[m+1])
      Bdata[j]=0;
    else if (BPdata[m]!=BPdata[m+1])
      Bdata[j]=1;
    j++;  
  }
  /*Serial.println("Biphase"); //print all biphase data
   for(int m=0; m<BPDsize;m++)
   Serial.print(BPdata[m]);
   Serial.println();  
   Serial.println("Binary"); //print all binary data
   for(int m=0; m<(j-1);m++)
   Serial.print(Bdata[m]);
   Serial.println();*/

  int n=7;
  while(n<j){
    //Find start of data by looking for this pattern: 000000001
    if(0==(Bdata[n]+Bdata[n-1]+Bdata[n-2]+Bdata[n-3]+Bdata[n-4]+Bdata[n-5]+Bdata[n-6]+Bdata[n-7]) && Bdata[n+1]==1 ){
      n++;
      //Find the end of the data by looking for 11111110
      int end1=0;
      while (end1==0){
        if(8==(Bdata[n]+Bdata[n+1]+Bdata[n+2]+Bdata[n+3]+Bdata[n+4]+Bdata[n+5]+Bdata[n+6]+Bdata[n+7]) && Bdata[n+8]==0){
          end1=n-1;
        }
        else
          n++;
      }
      //Using the location of the end point of the data, record appropriate 8 bits into array
      for(int p=0; p<=7; p++){
        bitWrite(mychar, p, Bdata[end1]);
        end1--;
      }
      n=1000; // get out of counting loop now
    }
    else
      n++;
  }
  //Serial.println(mychar,BIN); //print binary representation of character
  Serial.println(mychar);
}



