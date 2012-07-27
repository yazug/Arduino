#ifndef HMC5843_h
#define HMC5843_h


#if ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

class HMC5843 {
public:
	static const uint8_t HMC5843_ID	= 0x1E;  // 7-bit address

	static const uint8_t CRA	= 0x00;
	static const uint8_t CRB	= 0x01;
	static const uint8_t MR	= 0x02;
	static const uint8_t DOXMR	= 0x03;
	static const uint8_t SR	= 0x09;

	static const uint8_t Gain1620	= 0b000<<5;
	static const uint8_t Gain1300	= 0b001<<5;
	static const uint8_t Gain970	= 0b010<<5;
	static const uint8_t Gain780	= 0b011<<5;
	static const uint8_t Gain530	= 0b100<<5;
	static const uint8_t Gain460	= 0b101<<5;
	static const uint8_t Gain390	= 0b110<<5;
	static const uint8_t Gain280	= 0b111<<5;

	static const uint8_t Continuous_Conversion	= 0b00;
	static const uint8_t Single_Conversion		= 0b01;
	static const uint8_t Idle					= 0b10;
	static const uint8_t Sleep					= 0b11;

  public:
    HMC5843();
    void begin(const uint8_t gain = Gain1300, const uint8_t mode = Continuous_Conversion);
	void mode(const uint8_t mode);
	void gain(const uint8_t gain);
    void measured(int &x, int &y, int &z);
	int countsPerMilliGauss(const uint8_t gain);
};

#endif // HMC5843_h
