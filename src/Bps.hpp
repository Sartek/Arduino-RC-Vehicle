#ifndef Bps_hpp
#define Bps_hpp

#include <Arduino.h>
#include <i2c_t3.h>

class Bps
{
	public:
		Bps(unsigned char address);
		void setup();
		void readD1();
		void readD2();
		void readC();
		uint32_t getRawPressure();
		uint32_t getRawTemperature();
		
	private:
		unsigned char _address;
		//unsigned char _bpsData[7*2];
		bool _setup;
		unsigned char _state;
		uint32_t _pressure;
		uint32_t _temperature;
		uint16_t _PROM[8];
};

#endif