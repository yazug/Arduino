
struct Queue {
  static const int history_size = 8;
  union Event {
    struct {
      long date, time;
      byte type;
      byte NFCID[8];
      char CL[2];
      char PIN[8];
      char SUFFIX;
    };
    byte raw[32];
  } 
  history[history_size];
  int tally;
  int sindex;

  void init() {
    sindex = 0;
    tally = 0;
  }

  int count() { 
    return tally; 
  }

  void add(long dt, long tm, byte type, byte cid[], char pin[]) {
    int cindex = (sindex + tally) % history_size;
    history[cindex].date = dt;
    history[cindex].time = tm;
    history[cindex].type = type;
    for(int i = 0; i < 8; i++) {
      history[cindex].NFCID[i] = cid[i];
      history[cindex].PIN[i] = pin[i];
    }
    if ( tally == history_size ) { 
      sindex++;
      sindex %= history_size;
    } 
    else {
      tally++;
    }
  }

  int remove(int n) {
    Serial.println(tally);
    Serial.println(sindex);
    n = min(n, tally);
    sindex += n;
    sindex %= history_size;
    tally -= n;
    Serial.println(tally);
    Serial.println(sindex);
    return n;
  }

  Event & operator[](const int i) {
    int cindex = (sindex + i) % history_size;
    return history[cindex];
  }
};

