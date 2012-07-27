#include <WProgram.h>

template <class T>
class SerialStream : public Stream {
  
    virtual void write(uint8_t);
    using Print::write; // pull in write(str) and write(buf, size) from Print

public:
    SerialStream(){}

};

