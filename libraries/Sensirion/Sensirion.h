/* ========================================================================== */
/*                                                                            */
/*  Sensirion.h - Library for Sensirion SHT1x temperature and humidity      */ 
/*  sensor.                                                                   */
/*  Created by Markus Schatzl, November 28, 2008.                             */
/*  Released into the public domain.                                          */
/*                                                                            */
/* ========================================================================== */


#ifndef Sensirion_h
#define Sensirion_h

#include <inttypes.h>

class Sensirion
{
  private:
    uint8_t _pinData;
    uint8_t _pinClock;

    uint8_t softReset(void);
    uint8_t putByte(uint8_t);
    uint8_t readByte(uint8_t);
    uint8_t readSensorRaw(unsigned int*, uint8_t);
    
    float getDewpoint(float, float);
    
    void startTransmission(void);
    void resetConnection(void);
    void extractValues(float* ,float*);

  public:
    Sensirion(uint8_t, uint8_t);
    uint8_t measure(float* tf, float* hf , float* df);
    uint8_t measure(int * t, int * h);   
    uint8_t reset(void);
};

#endif
