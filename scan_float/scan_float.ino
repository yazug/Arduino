
void setup() {
  Serial.begin(9600);
  float f = scan_float("xxx89.2,39$", 2);
  Serial.println("Hi.");
  Serial.println(f);
}

void loop() {
}

float scan_float(char * p, const int n) {
  char * str = p;
  int i;
  float f = 0;
  for (i = 0; i <= n and *p != 0 ; i++) {
    while ( *p != '-' and !isdigit(*p) and *p != 0 )
      p++;
    //Serial.println(p);
    if ( i == n ) {
      f = atof(p);
      break;
    } else {
      while ( *p != 0 and (isdigit(*p) or *p == '-' or *p == '.') )
        p++;
    }
  }
  return f;
}




