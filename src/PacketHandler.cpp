#include "PacketHandler.hpp"
#include <Arduino.h>
#include "CircularBuffer.hpp"

CircularBuffer buffer;

void sendPacket(char packet[], unsigned int packetLength) {
    Serial.write(packet, packetLength);
    Serial.send_now();
}

unsigned int calculateChecksum(char packet[], unsigned int packetLength, unsigned int headerOffset) {
    unsigned char CK_A = 0;
    unsigned char CK_B = 0;

    for (unsigned int i = 0 + headerOffset; i < packetLength; i++) {
            CK_A = CK_A + packet[i];
            CK_B = CK_B + CK_A;
    }

    return (CK_A << 8 | CK_B);
}

bool verifyChecksum(char packet[], unsigned int packetLength, unsigned int headerOffset) {
    unsigned int checksum = calculateChecksum(packet, packetLength-2, headerOffset);
    return checksum == (unsigned int) (packet[packetLength-2] << 8 | packet[packetLength-1]);
}

void recieveData() {
	while(Serial.available()) {
        if(!buffer.isFull()) {
		    buffer.inputData(Serial.read());
        } else {
            processData();
        }
	}
}

void processData() {
    unsigned char dataLength = buffer.dataLength();
    unsigned char curDataByte;
    bool enoughData = true;

    while(enoughData) {
        dataLength = buffer.dataLength();
        if(dataLength > 0) {
            curDataByte = buffer.getData(0);
            if(curDataByte == 0xFC) {
                if(dataLength >= 5) {
                    curDataByte = buffer.getData(2);
                    if(curDataByte <= 123) {
                        if((curDataByte + 5) <= dataLength) {
                            char payloadLength = curDataByte;
                            char * data;
                            data = new char [payloadLength+5];
                            for(unsigned char i = 0; i < (3+payloadLength+2); i = i + 1) {
                                data[i] = buffer.getData(i);
                            }
                            if (verifyChecksum(data, payloadLength+5, 3)) {
                                //case Depends on packet type
                                switch(data[1]) {
                                    case 0x0A://mpu sensor data
                                        break;
                                    case 0x0B://mms sensor data
                                        break;
                                    case 0x0C://bps sensor data
                                        break;
                                    case 0x0D://gps data
                                        break;
                                    case 0x0F://controller input
                                        break;
                                    case 0xAA://Acknowledge
                                        break;
                                    case 0xFF://Negative Acknowledge
                                        break;
                                }

                                //remove packet from buffer
                                buffer.trimData(payloadLength+5);
                            } else {
                                //invalid Checksum
                                buffer.trimData(1);
                            }
                            delete[] data;
                        } else {
                            //NEED MORE DATA
                            enoughData = false;
                        }
                    } else {
                        //payloadLength too long to be valid packet
                        buffer.trimData(1);
                    }
                } else {
                    //NEED MORE DATA
                    enoughData = false;
                }
            } else {
                //not start of packet
                buffer.trimData(1);
            }
        } else {
            enoughData = false;
        }
    }
}