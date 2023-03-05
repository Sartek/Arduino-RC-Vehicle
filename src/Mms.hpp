#ifndef Mms_hpp
#define Mms_hpp

#include <Arduino.h>
#include <i2c_t3.h>

class Mms
{
    public:
        Mms(unsigned char address);
        void setup();
        void read();
        int16_t getMagneticX();
        int16_t getMagneticY();
        int16_t getMagneticZ();
        bool isDataReady();

    private:
        unsigned char _address;
        unsigned char _mmsData[3*2];
        bool _setup;
};

#endif