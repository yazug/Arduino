#include <stdlib.h> // for malloc and free
void* operator new(size_t size) { 
  return malloc(size); 
}
void operator delete(void* ptr) { 
  free(ptr); 
} 
void* operator new[](size_t size) { 
  return malloc(size); 
}
void operator delete[](void* ptr) { 
  free(ptr); 
} 

void setup() {
  struct d { 
    int x, y; 
  };
  d * tmp = new d;
  delete tmp;
  char *t = new char[6];
  delete [] t;
}

void loop() {
}


