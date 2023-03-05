#include "Mms.hpp"
#include <Arduino.h>
#include <i2c_t3.h>

Mms::Mms(unsigned char address) {
    _address = address;
    _setup = false;
    _mmsData[0] = 0;
    _mmsData[1] = 0;
    _mmsData[2] = 0;
    _mmsData[3] = 0;
    _mmsData[4] = 0;
    _mmsData[5] = 0;
}

void Mms::setup() {
    if (!_setup) {
        //Set CRA 0x00 value 0x78
        Wire.beginTransmission(_address);
        Wire.write(0x00);
        //Wire.write(0x78);
        Wire.write(0x00);
        Wire.endTransmission();
        //Set CRB (gain) 0x01 value 0x20
        Wire.beginTransmission(_address);
        Wire.write(0x01);
        Wire.write(0x20);
        //Wire.write(0xE0);
        Wire.endTransmission();
        //Set mode 0x02 value 0x01
        Wire.beginTransmission(_address);
        Wire.write(0x02);
        Wire.write(0x01);
        Wire.endTransmission();
    }

    _setup = true;
}

void Mms::read() {
    if (_setup) {
        //ReadValues 0x03-0x08
        Wire.beginTransmission(_address);
        Wire.write(0x03);
        //Wire.write(0x07);
        Wire.endTransmission();

        //Wire.requestFrom(static_cast<int>(_address), 6);
        Wire.requestFrom(_address, 6);
        _mmsData[0] = Wire.read();
        _mmsData[1] = Wire.read();
        _mmsData[2] = Wire.read();
        _mmsData[3] = Wire.read();
        _mmsData[4] = Wire.read();
        _mmsData[5] = Wire.read();
        //Set mode 0x02 value 0x01
        Wire.beginTransmission(_address);
        Wire.write(0x02);
        Wire.write(0x01);
        Wire.endTransmission();
    }
}

int16_t Mms::getMagneticX() {
    return (_mmsData[0] << 8 | _mmsData[1]);
}

int16_t Mms::getMagneticY() {
    return (_mmsData[4] << 8 | _mmsData[5]);
}

int16_t Mms::getMagneticZ() {
    return (_mmsData[2] << 8 | _mmsData[3]);
}

bool Mms::isDataReady() {
    bool dataReadyBit = false;
    //read status register 0x09
    Wire.beginTransmission(_address);
    Wire.write(0x09);
    Wire.endTransmission();

    //Wire.requestFrom(static_cast<int>(_address), 1);
    Wire.requestFrom(_address, 1);
    //Set dataReadyBit to last bit of register value.
    dataReadyBit = Wire.read() & 0x01;

    return dataReadyBit;
}