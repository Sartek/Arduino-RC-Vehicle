#include <Arduino.h>
#include "Bps.hpp"
#include <i2c_t3.h>

Bps::Bps(unsigned char address) {
	_address = address;
	_setup = false;
    _state = 0;
    _pressure = 0;
    _temperature = 0;
    _PROM[0] = 0;
    _PROM[1] = 0;
    _PROM[2] = 0;
    _PROM[3] = 0;
    _PROM[4] = 0;
    _PROM[5] = 0;
    _PROM[6] = 0;
    _PROM[7] = 0;
}

void Bps::setup() {
	if (!_setup) {
        //Reset
	    Wire.beginTransmission(_address);
	    Wire.write(0x1E); // RESET COMMAND 1E / 00001110
	    Wire.endTransmission();
	    delay(3);

	    // READ PROM
	    // Read PROM command address 000 Factory Reserved A0 / 10100000
	    Wire.beginTransmission(_address);
	    Wire.write(0xA0);
	    Wire.endTransmission();

	    Wire.requestFrom(_address, 2);
	    _PROM[0] = Wire.read() << 8 | Wire.read();

	    // Read PROM command address 001 Coefficient 1 A2 / 10100010
	    Wire.beginTransmission(_address);
	    Wire.write(0xA2);
	    Wire.endTransmission();

	    Wire.requestFrom(_address, 2);
	    _PROM[1] = Wire.read() << 8 | Wire.read();

	    // Read PROM command address 010 Coefficient 2 A4 / 10100100
	    Wire.beginTransmission(_address);
	    Wire.write(0xA4);
	    Wire.endTransmission();

	    Wire.requestFrom(_address, 2);
	    _PROM[2] = Wire.read() << 8 | Wire.read();

	    // Read PROM command address 011 Coefficient 3 A6 / 10100110
	    Wire.beginTransmission(_address);
	    Wire.write(0xA6);
	    Wire.endTransmission();

	    Wire.requestFrom(_address, 2);
	    _PROM[3] = Wire.read() << 8 | Wire.read();

	    // Read PROM command address 100 Coefficient 4 A8 / 10101000
	    Wire.beginTransmission(_address);
	    Wire.write(0xA8);
	    Wire.endTransmission();

	    Wire.requestFrom(_address, 2);
	    _PROM[4] = Wire.read() << 8 | Wire.read();

	    // Read PROM command address 101 Coefficient 5 AA / 10101010
	    Wire.beginTransmission(_address);
	    Wire.write(0xAA);
	    Wire.endTransmission();

	    Wire.requestFrom(_address, 2);
	    _PROM[5] = Wire.read() << 8 | Wire.read();

	    // Read PROM command address 110 Coefficient 6 AC / 10101100
	    Wire.beginTransmission(_address);
	    Wire.write(0xAC);
	    Wire.endTransmission();

	    Wire.requestFrom(_address, 2);
	    _PROM[6] = Wire.read() << 8 | Wire.read();

	    // Read PROM command address 111 4 bit CRC AE / 10101110
	    Wire.beginTransmission(_address);
	    Wire.write(0xAE);
	    Wire.endTransmission();

	    Wire.requestFrom(_address, 2);
	    _PROM[7] = Wire.read() << 8 | Wire.read();
	}
	
	_setup = true;
}

void Bps::readD1() {
	if (_setup) {
        Wire.beginTransmission(_address);
	    Wire.write(0x48); //Convert D1 command
	    Wire.endTransmission();
	    _state = 1;
	}
}

void Bps::readD2() {
	if (_setup) {
	    Wire.beginTransmission(_address);
	    Wire.write(0x58); //Convert D2 command
	    Wire.endTransmission(true);
	    _state = 2;
	}
}

void Bps::readC() {
	if (_setup) {
	    Wire.beginTransmission(_address);
	    Wire.write(0x00); //Read value
	    Wire.endTransmission();

	    Wire.requestFrom(_address, 3);
	    if (_state == 1) {
	    	_pressure = Wire.read() << 16 | Wire.read() << 8 | Wire.read();
	    } else if (_state == 2) {
	    	_temperature = Wire.read() << 16 | Wire.read() << 8 | Wire.read();
	    }

	    _state = 0;
	}
}

uint32_t Bps::getRawPressure() {
    return(_pressure);
}

uint32_t Bps::getRawTemperature() {
    return(_temperature);
}