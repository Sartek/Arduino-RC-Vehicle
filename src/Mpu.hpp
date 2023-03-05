#ifndef Mpu_hpp
#define Mpu_hpp

#include <Arduino.h>
#include <i2c_t3.h>

class Mpu
{
	public:
		Mpu(unsigned char address);
		void setup();
		void read();
		int16_t getAccelX();
		int16_t getAccelY();
		int16_t getAccelZ();
		int16_t getTemperature();
		int16_t getGyroX();
		int16_t getGyroY();
		int16_t getGyroZ();

		
	private:
		unsigned char _address;
		unsigned char _mpuData[7*2];
		bool _setup;
};

#endif