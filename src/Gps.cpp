#include "Gps.hpp"
#include <Arduino.h>
#include "CircularBuffer.hpp"

Gps::Gps() {
    _fixReady = false;

    for( unsigned char i = 0; i < sizeof(_gpsData); i = i + 1 ) {
        _gpsData[i] = 0x00;
    }
}

bool Gps::isFixReady() {
    return _fixReady;
}

void Gps::inputData(unsigned char data) {
    if(!_gpsBuffer.isFull()) {
        _gpsBuffer.inputData(data);
    } else {
        Gps::processData();
        _gpsBuffer.inputData(data);
    }
}

void Gps::processData() {

    //look for 0xb5 from tail to head
    for( unsigned char i = 0; i < _gpsBuffer.dataLength(); i = i + 1) {
        if(_gpsBuffer.getData(i) == 0xb5) {
            break;
        }
        _gpsBuffer.trimData(1);
    }

    unsigned char trimLength = 0;

    //make sure first byte is 0xb5
    if(_gpsBuffer.getData(0) == 0xb5) {
        //look for 0x62 next byte
        if((_gpsBuffer.dataLength() >= 2) & (_gpsBuffer.getData(1) == 0x62)) {
            //look for 0x01 next byte
            if((_gpsBuffer.dataLength() >= 3) & (_gpsBuffer.getData(2) == 0x01)) {
                //look for 0x07 next byte
                if((_gpsBuffer.dataLength() >= 4) & (_gpsBuffer.getData(3) == 0x07)) {
                    //look for 0x54 next byte - payload length #1
                    if((_gpsBuffer.dataLength() >= 5) & (_gpsBuffer.getData(4) == 0x54)) {
                        uint16_t payloadLength = 0;
                        payloadLength = _gpsBuffer.getData(4);
                        //look for 0x00 next byte - payload length #2
                        if((_gpsBuffer.dataLength() >= 6) & (_gpsBuffer.getData(5) == 0x00)) {
                            payloadLength = _gpsBuffer.getData(5) << 8;
                            //look for CK_A
                            if(_gpsBuffer.dataLength() >= (6+payloadLength+1)) {
                                uint16_t checksum = 0;
                                checksum = _gpsBuffer.getData(5+payloadLength+1) << 8;
                                //look for CK_B next byte
                                if(_gpsBuffer.dataLength() >= (6+payloadLength+2)) {
                                    checksum = checksum | _gpsBuffer.getData(5+payloadLength+2);
                                    //Verify Checksum
                                    unsigned char CK_A = 0;
                                    unsigned char CK_B = 0;

                                    for (unsigned char i = 0 + 2; i < (_gpsBuffer.dataLength()-2); i = i + 1) {
                                            CK_A = CK_A + _gpsBuffer.getData(i);
                                            CK_B = CK_B + CK_A;
                                    }

                                    if(checksum == (CK_A << 8 | CK_B)) {
                                        trimLength = 6+payloadLength+2;

                                        for(unsigned char i = 0; i < (6+payloadLength+2); i = i + 1) {
                                            _gpsData[i] =_gpsBuffer.getData(i);
                                        }
                                    } else {
                                        trimLength = 6;
                                    }
                                }

                            }

                        } else if (_gpsBuffer.dataLength() >= 6) {
                            trimLength = 6;
                        }
                    } else if (_gpsBuffer.dataLength() >= 5) {
                        trimLength = 5;
                    }
                } else if (_gpsBuffer.dataLength() >= 4) {
                    trimLength = 4;
                }
            } else if (_gpsBuffer.dataLength() >=3) {
                trimLength = 3;
            }

        } else if (_gpsBuffer.dataLength() >=2) {
            trimLength =  2;
        }
    }
    _gpsBuffer.trimData(trimLength);

}