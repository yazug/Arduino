struct Combination {
  int * combi;
  char ** labels;
  int dim;
  int nodes;

  Combination(int dims, char * strs[]) {
    combi = (int*) malloc(sizeof(int)*dims);
    dim = dims;
    labels = strs;
    for (nodes = 0; labels[nodes] != NULL; nodes++);
    reset();
  }

  ~Combination() {
    free(combi);
  }
  void reset() {
    for (int i = 0; i < dim; i++) {
      combi[i] = 0;
    }
  }

  boolean hasNext() {
    int i;
    for (i = 0; i < dim && combi[i]+1 == nodes; i++) ;
    return i < dim;
  }

  boolean next() {
    int i;
    for (i = 0; i < dim; i++) {
      if ( combi[i]+1 < nodes ) {
        combi[i]++;
        break;
      } 
      else {
        combi[i] = 0;
      }
    }
    return i < dim;
  }

  void printOnSerial() {
    Serial.println();
    Serial.print("Combination: ");
    for (int i = 0; i < dim; i++) {
      Serial.print(labels[combi[i]]);
      Serial.print(", ");
    }
    Serial.println();    
  }
};

char * a[] = {
  "NO", "YES", 0};
Combination combi(7, a);

void setup() {

  Serial.begin(9600);
}

void loop() {
  combi.printOnSerial();  
  if (combi.hasNext()) {
    Serial.println("going to the next..");
    combi.next();
  } 
  else {
    Serial.println("finished.");
    for(;;);
  }  
  delay(200);

}


