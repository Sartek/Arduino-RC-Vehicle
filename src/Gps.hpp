#ifndef Gps_hpp
#define Gps_hpp

#include <Arduino.h>
#include "CircularBuffer.hpp"

class Gps
{
    public:
        Gps();
        bool isFixReady();
        void inputData(unsigned char data);
        void processData();
        unsigned char _gpsData[128];

    private:

        //unsigned char _gpsData[128];
        CircularBuffer _gpsBuffer;
        bool _fixReady;
};

#endif