

int best(int price[], int items, int budget, boolean buying[]) {
  int itemNo = items - 1;
  if ( items == 0 )
    return 0;
  if ( items == 1 and price[itemNo] <= budget )
    return price[itemNo];

  
  int buy = price[itemNo] 
    + best(price, itemNo, budget - price[itemNo], buying);
  int notbuy = best(price, itemNo, budget, buying);
  if ( buy <= budget and buy >= notbuy ) {
    buying[itemNo] = true;
    return buy;
  } else {
    buying[itemNo] = false;
    return notbuy;
  }
}

int list0[] = { 
  108, 78, 78, 58, 58, 68, 0};
int list1[] = { 
  108, 138, 78, 78, 58, 58, 68, 128, 98, 158, 118, 128, 328, 
  228, 238, 198, 298, 64, 178, 0};
int * plist = list1;
const int B = 500;

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("Good day.");
  
  Serial.print("Budget: ");
  Serial.println(B);
  Serial.println("Items: ");
  int number;
  for ( number = 0; plist[number] > 0 ; number++) {
    Serial.print(plist[number]);
    Serial.print(", ");
  }
  Serial.println();
  Serial.print("Number of items: ");
  Serial.println(number);
  Serial.println();
  
  boolean buyingGuide[number];
  
  long swatch_milli = millis();
  long swatch_mu = micros();
  int result = best(plist, number, B, buyingGuide);
  swatch_mu = micros() - swatch_mu;
  swatch_milli = millis() - swatch_milli;
  
  Serial.print("Recommended total price: ");
  Serial.println(result);
  Serial.print("with items ");
  for ( number = 0; plist[number] > 0 ; number++) {
    if ( buyingGuide[number] ) {
      Serial.print(number);
      Serial.print(" ");
    }
  }
  Serial.println(".");
  
  Serial.println("Elapsed time: ");
  Serial.print(swatch_milli);
  if ( swatch_milli == (swatch_mu/1000) ) {
    Serial.print(" milli ");
    Serial.print(swatch_mu % 1000);
    Serial.println(" micro secs. ");
  } else {
    Serial.println(" milli secs. ");
  }
}

void loop() {
}





