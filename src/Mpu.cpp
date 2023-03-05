#include "Mpu.hpp"
#include <Arduino.h>
#include <i2c_t3.h>

Mpu::Mpu(unsigned char address) {
	_address = address;
	_setup = false;
	_mpuData[0]  = 0;   //0x3B (ACCEL_XOUT_H)
	_mpuData[1]  = 0;   //0x3C (ACCEL_XOUT_L)
	_mpuData[2]  = 0;   //0x3D (ACCEL_YOUT_H)
	_mpuData[3]  = 0;   //0x3E (ACCEL_YOUT_L)
	_mpuData[4]  = 0;   //0x3F (ACCEL_ZOUT_H)
	_mpuData[5]  = 0;   //0x40 (ACCEL_ZOUT_L)
	_mpuData[6]  = 0;   //0x41 (TEMP_OUT_H)
	_mpuData[7]  = 0;   //0x42 (TEMP_OUT_L)
	_mpuData[8]  = 0;   //0x43 (GYRO_XOUT_H)
	_mpuData[9]  = 0;   //0x44 (GYRO_XOUT_L)
	_mpuData[10] = 0;  //0x45 (GYRO_YOUT_H)
	_mpuData[11] = 0;  //0x46 (GYRO_YOUT_L)
	_mpuData[12] = 0;  //0x47 (GYRO_ZOUT_H)
	_mpuData[13] = 0;  //0x48 (GYRO_ZOUT_L)
}

void Mpu::setup() {
	if (!_setup) {
		Wire.beginTransmission(_address);
		Wire.write(0x6B);  // PWR_MGMT_1 register
		Wire.write(0);     // set to zero (wakes up the MPU-6050) Bit order -DEVICE_RESET-SLEEP-CYCLE-blank-TEMP_DIS-CLKSEL[2:0]-CLKSEL[2:0]
		Wire.endTransmission(true);
		
		// Reset
		Wire.beginTransmission(_address);
		Wire.write(0x6B);  // PWR_MGMT_1 register
		Wire.write(0x40);  //set to b1000000 (resets the MPU-6050) Bit order -DEVICE_RESET-SLEEP-CYCLE-blank-TEMP_DIS-CLKSEL[2:0]-CLKSEL[2:0]
		Wire.endTransmission(true);
		
		delay(10);
		
		Wire.beginTransmission(_address);
		Wire.write(0x6a);
		Wire.write(0x00);
		Wire.endTransmission(true);

		Wire.beginTransmission(_address);
		Wire.write(0x37);
		Wire.write(0x02);
		Wire.endTransmission(true);

		// Set powermode again
		Wire.beginTransmission(_address);
		Wire.write(0x6B);  // PWR_MGMT_1 register
		Wire.write(0);     // set to zero (wakes up the MPU-6050) Bit order -DEVICE_RESET-SLEEP-CYCLE-blank-TEMP_DIS-CLKSEL[2:0]-CLKSEL[2:0]
		Wire.endTransmission(true);
	}
	
	_setup = true;
}

void Mpu::read() {
	if (_setup) {
		Wire.beginTransmission(_address);
		Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H).
		Wire.endTransmission(false);
		
		Wire.requestFrom(_address, 14, true); // request a total of 14 registers
		_mpuData[0] = Wire.read();   //0x3B (ACCEL_XOUT_H)
		_mpuData[1] = Wire.read();   //0x3C (ACCEL_XOUT_L)
		_mpuData[2] = Wire.read();   //0x3D (ACCEL_YOUT_H)
		_mpuData[3] = Wire.read();   //0x3E (ACCEL_YOUT_L)
		_mpuData[4] = Wire.read();   //0x3F (ACCEL_ZOUT_H)
		_mpuData[5] = Wire.read();   //0x40 (ACCEL_ZOUT_L)
		_mpuData[6] = Wire.read();   //0x41 (TEMP_OUT_H)
		_mpuData[7] = Wire.read();   //0x42 (TEMP_OUT_L)
		_mpuData[8] = Wire.read();   //0x43 (GYRO_XOUT_H)
		_mpuData[9] = Wire.read();   //0x44 (GYRO_XOUT_L)
		_mpuData[10] = Wire.read();  //0x45 (GYRO_YOUT_H)
		_mpuData[11] = Wire.read();  //0x46 (GYRO_YOUT_L)
		_mpuData[12] = Wire.read();  //0x47 (GYRO_ZOUT_H)
		_mpuData[13] = Wire.read();  //0x48 (GYRO_ZOUT_L)
	}
}

int16_t Mpu::getAccelX() {
    return (_mpuData[0] << 8 | _mpuData[1]);
}

int16_t Mpu::getAccelY() {
    return (_mpuData[2] << 8 | _mpuData[3]);
}

int16_t Mpu::getAccelZ() {
    return (_mpuData[4] << 8 | _mpuData[5]);
}

int16_t Mpu::getTemperature() {
    return (_mpuData[6] << 8 | _mpuData[7]);
}

int16_t Mpu::getGyroX() {
    return (_mpuData[8] << 8 | _mpuData[9]);
}

int16_t Mpu::getGyroY() {
    return (_mpuData[10] << 8 | _mpuData[11]);
}

int16_t Mpu::getGyroZ() {
    return (_mpuData[12] << 8 | _mpuData[13]);
}