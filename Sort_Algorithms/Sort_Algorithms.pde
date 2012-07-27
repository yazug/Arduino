struct string {
  const static int LimitLength = 64;

  char str[LimitLength];
  int length;

  string() {
    initialize();
  }

  void initialize() {
    length = 0;
    str[0] = 0;
  }

  void initialize(string & s) {
    memcpy(str, s.str,s.length+1);
    length = s.length;
  }

  int append(char c) {
    if ( !(length < LimitLength) )
      return length;
    str[length] = c;
    length++;
    str[length] = 0;
    return length;
  }

  boolean isEmpty() {
    return length == 0;
  }
} 
myInput, inputcopy;

void setup() {
  Serial.begin(9600);
  Serial.println("Ready.");

}

void loop() {
  long stopwatch;

  while (Serial.available()) {
    myInput.append(Serial.read());
    if ( Serial.available() )
      continue;
    delay(10);
  }

  if (!myInput.isEmpty()) {
    inputcopy.initialize(myInput);
    Serial.print("======= No. ");
    Serial.print(millis());
    Serial.println(" ======= ");
    Serial.print("Before sorting ");
    Serial.print(myInput.length);
    Serial.println(" elements:");
    Serial.print("\'");
    Serial.print(myInput.str);
    Serial.print("\'");
    Serial.println(" elements:");
    //
    stopwatch = micros();
    SelectionSort(myInput.str, myInput.length);
    stopwatch = micros() - stopwatch;
    //
    Serial.println();
    Serial.print("It took ");
    Serial.print(stopwatch);
    Serial.println(" micro seconds, ");
    Serial.println("After sorting: ");
    Serial.println(myInput.str);
    Serial.println();
    //
    stopwatch = micros();
    BubbleSort(inputcopy.str, myInput.length);
    stopwatch = micros() - stopwatch;
    //
    Serial.println();
    Serial.print("It took ");
    Serial.print(stopwatch);
    Serial.println(" micro seconds, ");
    Serial.println("After sorting: ");
    Serial.println(inputcopy.str);
    Serial.println();
    //
    //
    myInput.initialize();
  }
}

void SelectionSort(char array[], int size) {
  int i, j;
  char tmp;

  for (j = 0; j < size-1; j++) {
    for (i = j; i < size; i++) {
      tmp = array[i];
      if ( tmp > array[j] ) {
        array[i] = array[j];
        array[j] = tmp;
      }
    }
  }
}

void BubbleSort(char array[], int n) {
  int i, j;
  char tmp;

  for (i = 0; i+1 < n; i++) {
    for (j = 0; j+1 < n; j++) {
      if ( array[j+1] > array[j] ) {
        tmp = array[j];
        array[j] = array[j+1];
        array[j+1] = tmp;
      }
    }
  }
}




