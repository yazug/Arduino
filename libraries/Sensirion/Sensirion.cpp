/* ========================================================================== */
/*                                                                            */
/*  Sensirion.cpp - Library for Sensirion SHT1x temperature and humidity      */ 
/*  sensor.                                                                   */
/*  Created by Markus Schatzl, November 28, 2008.                             */
/*  Released into the public domain.                                          */
/*                                                                            */
/* ========================================================================== */


/******************************************************************************
 * Includes
 ******************************************************************************/

extern "C" {
  // AVR LibC Includes
  #include <inttypes.h>
  #include <stdlib.h>
  #include <math.h>

  // Wiring Core Includes
//  #undef abs
  #include "WConstants.h"
//#include <WProgram.h>
}

#include "Sensirion.h"


/******************************************************************************
 * Definitions
 ******************************************************************************/

// register dr command r/w
#define REG_STATUS_W 0x06 //000 0011 0
#define REG_STATUS_R 0x07 //000 0011 1
#define REG_RESET    0x1e //000 1111 0

#define MODE_TEMP    0x03 //000 0001 1
#define MODE_HUMI    0x05 //000 0010 1

// Pulse lengths
#define PULSE_LONG delayMicroseconds(3) 
#define PULSE_SHORT delayMicroseconds(1)


/******************************************************************************
 * Constructors
 ******************************************************************************/

Sensirion::Sensirion(uint8_t data, uint8_t clock)
{
  // Pins for software SPI
  _pinData = data;
  _pinClock = clock;
  
  // set DDR for software SPI pins
  pinMode(_pinClock, OUTPUT);
  pinMode(_pinData, OUTPUT);
  
  softReset();
}

/******************************************************************************
 * User functions
 ******************************************************************************/
  
// TODO: Return result in a more meaningful way (errors)
uint8_t Sensirion::measure(float *temp, float *humi, float *dew)
{
  uint8_t error;
  unsigned int humival_raw, tempval_raw;
  float humival_f, tempval_f, dew_point_f;
  // char str[15];
  
  // Measure temperature
  if ((error = readSensorRaw( &tempval_raw, 0)) != 0)
    return(error);
    
  // Measure humidity
  if ((error = readSensorRaw( &humival_raw, 1)) != 0)
    return(error);
  
  /*
    (error==1): "SHT1x sensor communication error"
    (error==3): "SHT1x sensor timeout error";
  */

  humival_f = (float) humival_raw;
  tempval_f = (float) tempval_raw;
	
  // Reinterpret sensor data			
  extractValues(&humival_f, &tempval_f);
  // Calculate dew point
  dew_point_f = getDewpoint(humival_f, tempval_f);
  
  *temp = tempval_f;
  *humi = humival_f;
  *dew  = dew_point_f;
    
  return(0);    
}


//
uint8_t Sensirion::measure(int * temp, int * humid) {
	uint8_t error;

	const float c1= -4.0;             // for 12 Bit
	const float c2= +0.0405;          // for 12 Bit
	const float c3= -0.0000028;       // for 12 Bit
	const float T1= +0.01;            // for 14 Bit @ 5V
	const float T2= +0.00008;         // for 14 Bit @ 5V	

	unsigned int tticks, hticks;
	unsigned long hlong;
	
	// Measure temperature
	if ((error = readSensorRaw( &tticks, 0)) != 0)
		return(error);
    
	// Measure humidity
	if ((error = readSensorRaw( &hticks, 1)) != 0)
		return(error);
	/*
	 (error==1): "SHT1x sensor communication error"
	 (error==3): "SHT1x sensor timeout error";
	 */
	
	*temp =  - 4010 /* 4.01 @ 5V */ + /* 0.01 * */ tticks;
			//from ticks [14bit] to temperature [°C]
	hlong = hticks;
	hlong = -400 /* -4.0 */ + 4.05 /* 0.0405 */ * hlong
				-0.28672 /* -2.8000E-6 * 100 * 1024 */ * (sq(hlong) >> 10);
			// from ticks [12bit] to RH_linear, V3
	// temperature compensation
	hlong = ((float)(*temp)/100 - 25.0) * (1 /* 0.01 */ + 0.08 /* 0.00008*/ * hticks) + hlong;
	*humid = min(max(hlong, 0), 10000);

	return(0);    
}



// TODO: Should probably return boolean
uint8_t Sensirion::reset(void)
{
  return softReset();
}

/******************************************************************************
 * Sensirion SHT10 data communication
 ******************************************************************************/

// Start a measurement (humidity/temperature) with checksum
// p_value returns 2 bytes
// Modes: 1 = humidity  0 = temperature
// Return value: 1 = write error, 2 = crc error, 3 = timeout
uint8_t Sensirion::readSensorRaw(unsigned int *p_value, uint8_t mode)
{
        uint8_t i = 0;
        startTransmission(); //transmission start
        *p_value = 0;
        if (mode){
                mode = MODE_HUMI;
        } else {
                mode = MODE_TEMP;
        }
        if (putByte(mode)) {
                return(1);
        }
        // normal delays: temp i=70, humi i=20
        while (i < 240){
                delay(3);
                if (digitalRead(_pinData) == 0){
                        i = 0;
                        break;
                }
                i++;
        }
        if (i) {
                // or timeout 
                return(3);
        }
        i = readByte(1); //read the first byte (MSB)
        *p_value = (i << 8) | readByte(0); //read the second byte (LSB) and end with no-ack
        return(0);
}

// Writes a byte on the Sensibus and checks the acknowledge
uint8_t Sensirion::putByte(uint8_t data)
{
        uint8_t i = 0x80;
        uint8_t error = 0;
        // Set data line for output
        pinMode(_pinData, OUTPUT);
        while(i){ //shift bit for masking
                if (i & data) {
                //masking value with i , write to SENSI-BUS
                        digitalWrite(_pinData, HIGH); 
                }else{ 
                        digitalWrite(_pinData, LOW);
                }
                digitalWrite(_pinClock, HIGH); //clk for SENSI-BUS
                PULSE_LONG;
                digitalWrite(_pinClock, LOW);
                PULSE_SHORT;
                i=(i>>1);
        }
        // Set back data line for input
        pinMode(_pinData, INPUT);
        // PULLUP1;
        digitalWrite(_pinClock, HIGH); //clk #9 for ack
        PULSE_LONG;
        if (digitalRead(_pinData)){ //check ack (DATA will be pulled down by SHT11)
                error=1;
        }
        PULSE_SHORT;
        digitalWrite(_pinClock, LOW);
        return(error); //error=1 in case of no acknowledge
}


// Reads a byte form the Sensibus and returns an acknowledge in case of "ack=1"
uint8_t Sensirion::readByte(uint8_t ack)
{
        uint8_t i = 0x80;
        uint8_t val = 0;
        pinMode(_pinData, INPUT); //release DATA-line
        // PULLUP1;
        while(i){ //shift bit for masking
                digitalWrite(_pinClock, HIGH); //clk for SENSI-BUS
                PULSE_SHORT;
                if (digitalRead(_pinData)){
                        val=(val | i); //read bit
                }
                digitalWrite(_pinClock, LOW);
                PULSE_SHORT;
                i=(i>>1);
        }
        pinMode(_pinData, OUTPUT);
        if (ack){
                //in case of "ack==1" pull down DATA-Line
                digitalWrite(_pinData, LOW);
        }else{
                digitalWrite(_pinData, HIGH);
        }
        digitalWrite(_pinClock, HIGH); //clk #9 for ack
        PULSE_LONG;
        digitalWrite(_pinClock, LOW);
        PULSE_SHORT;
        pinMode(_pinData, INPUT);
        // PULLUP1;
        return (val);
}


/******************************************************************************
 * Sensirion SHT10 signalling
 ******************************************************************************/

// generates a sensirion specific transmission start
// This is the point where sensirion is not I2C standard conform and the
// main reason why the AVR TWI hardware support can not be used.
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
void Sensirion::startTransmission(void)
{      
        digitalWrite(_pinClock, LOW);
        pinMode(_pinData, OUTPUT); 
        digitalWrite(_pinData, HIGH);
        PULSE_SHORT;
        digitalWrite(_pinClock, HIGH);
        PULSE_SHORT;
        digitalWrite(_pinData, LOW);
        PULSE_SHORT;
        digitalWrite(_pinClock, LOW);
        PULSE_LONG;
        digitalWrite(_pinClock, HIGH);
        PULSE_SHORT;
        digitalWrite(_pinData, HIGH);
        PULSE_SHORT;
        digitalWrite(_pinClock, LOW);
        PULSE_SHORT;
        //
        pinMode(_pinData, INPUT);
        //PULLUP1;
}

// Communication reset 
// DATA-line=1 and at least 9 SCK cycles followed by transstart
//      _____________________________________________________         ________
// DATA:                                                     |_______|
//          _    _    _    _    _    _    _    _    _        ___    ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|  |___|   |______
void Sensirion::resetConnection(void)
{
        uint8_t i;
        
        digitalWrite(_pinClock, LOW);
        pinMode(_pinData, OUTPUT);
        digitalWrite(_pinData, HIGH);
        
        for (i = 0; i < 9; i++) {
                digitalWrite(_pinClock, HIGH);
                PULSE_LONG;
                digitalWrite(_pinClock, LOW);
                PULSE_LONG;
        }
        startTransmission();
}

// Resets the sensor by a soft reset
uint8_t Sensirion::softReset(void)
{
  // Reset communication
  resetConnection(); 
  //send RESET-command to sensor, return = 1 in case of no response
  return (putByte(REG_RESET));
}


/******************************************************************************
 * Helper Functions
 ******************************************************************************/

// Calculates temperature [°C] and humidity [%RH] 
// Input :  humidity    [Ticks] (12 bit) 
//          temperature [Ticks] (14 bit)
// Output:  humidity    [%RH]
//          temperature [°C]
void Sensirion::extractValues(float *p_humidity ,float *p_temperature)
{ 
  const float C1= -4.0;             // for 12 Bit
  const float C2= +0.0405;          // for 12 Bit
  const float C3= -0.0000028;       // for 12 Bit
  const float T1= +0.01;            // for 14 Bit @ 5V
  const float T2= +0.00008;         // for 14 Bit @ 5V	

  float rh = *p_humidity;           // rh:      Humidity [Ticks] 12 Bit 
  float t = *p_temperature;         // t:       Temperature [Ticks] 14 Bit
  float rh_lin;                     // rh_lin:  Humidity linear
  float rh_true;                    // rh_true: Temperature compensated humidity
  float t_C;                        // t_C   :  Temperature [°C]

  t_C = t * 0.01 - 40;              //calc. temperature from ticks to [°C]
  rh_lin = C3*rh*rh + C2*rh + C1;   //calc. humidity from ticks to [%RH]
  rh_true = (t_C-25)*(T1+T2*rh)+rh_lin; //calc. temperature compensated humidity [%RH]
  if (rh_true > 100) rh_true=100;   //cut if the value is outside of
  if (rh_true < 0.1) rh_true=0.1;   //the physical possible range

  *p_temperature = t_C;             //return temperature [°C]
  *p_humidity = rh_true;            //return humidity[%RH]
}

// Calculates dew point
// Input:   humidity [%RH], temperature [°C]
// Output:  dew point [°C]
float Sensirion::getDewpoint(float h, float t)
{ 
  float logEx, dew_point;
  logEx = 0.66077 + 7.5 * t / (237.3 + t) + (log10(h) - 2);
  dew_point = (logEx - 0.66077) * 237.3 / (0.66077 + 7.5 - logEx);
  return dew_point;
}
