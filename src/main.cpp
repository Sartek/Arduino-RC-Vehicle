#include <Arduino.h>
#include "Mpu.hpp"
#include "Bps.hpp"
#include "Mms.hpp"
#include "Gps.hpp"
#include "Packethandler.hpp"
#include <i2c_t3.h>

#define mpuAddress 0x68 
#define bpsAddress 0x77
#define mmsAddress 0x1E

//Servos = 23-20

Mpu mpu(mpuAddress);
Bps bps(bpsAddress);
Mms mms(mmsAddress);
Gps gps;

elapsedMicros mpuTimer;
elapsedMicros bpsTimer;
elapsedMicros mmsTimer;

elapsedMicros loopTimer;
unsigned int loopsPerSecond = 0;

char bpsState = 0;

void sendDataPacket(char packetType) {
	char * serialPacket;
	char serialPacketLength = 0;
	unsigned int checksum;
	switch (packetType) {
		//mpu
		case 1:
			int16_t accelX, accelY, accelZ, mpuTemp, gyroX, gyroY, gyroZ;
			serialPacket = new char [19];
			accelX = mpu.getAccelX();
			accelY = mpu.getAccelY();
			accelZ = mpu.getAccelZ();
			mpuTemp = mpu.getTemperature();
			gyroX = mpu.getGyroX();
			gyroY = mpu.getGyroY();
			gyroZ = mpu.getGyroZ();

			serialPacket[0] = 0xFC;
			serialPacket[1] = 0x0A;
			serialPacket[2] = 14;//data length
			serialPacket[3] = accelX >> 8;
			serialPacket[4] = accelX & 0xFF;
			serialPacket[5] = accelY >> 8;
			serialPacket[6] = accelY & 0xFF;
			serialPacket[7] = accelZ >> 8;
			serialPacket[8] = accelZ & 0xFF;
			serialPacket[9] = mpuTemp >> 8;
			serialPacket[10] = mpuTemp & 0xFF;
			serialPacket[11] = gyroX >> 8;
			serialPacket[12] = gyroX & 0xFF;
			serialPacket[13] = gyroY >> 8;
			serialPacket[14] = gyroY & 0xFF;
			serialPacket[15] = gyroZ >> 8;
			serialPacket[16] = gyroZ & 0xFF;
			serialPacketLength = 17;
			checksum = calculateChecksum(serialPacket, serialPacketLength, 3);
			serialPacket[17] = checksum >> 8;
			serialPacket[18] = checksum & 0xFF;
			serialPacketLength = 19;
			//verifyChecksum(serialPacket, serialPacketLength, 3);
			break;
		//mms
		case 2:
			int16_t magnoX, magnoY, magnoZ;
			serialPacket = new char [11];
			magnoX = mms.getMagneticX();
			magnoY = mms.getMagneticY();
			magnoZ = mms.getMagneticZ();
			serialPacket[0] = 0xFC;
			serialPacket[1] = 0x0B;
			serialPacket[2] = 6;//data length
			serialPacket[3] = magnoX >> 8;
			serialPacket[4] = magnoX & 0xFF;
			serialPacket[5] = magnoY >> 8;
			serialPacket[6] = magnoY & 0xFF;
			serialPacket[7] = magnoZ >> 8;
			serialPacket[8] = magnoZ & 0xFF;
			serialPacketLength = 9;
			checksum = calculateChecksum(serialPacket, serialPacketLength, 3);
			serialPacket[9] = checksum >> 8;
			serialPacket[10] = checksum & 0xFF;
			serialPacketLength = 11;
			//verifyChecksum(serialPacket, serialPacketLength, 3);
			break;
		//bps
		case 3:
			uint32_t bpsRawPressure, bpsRawTemperature;
			serialPacket = new char [13];
			bpsRawPressure = bps.getRawPressure();
			bpsRawTemperature = bps.getRawTemperature();

			serialPacket[0] = 0xFC;
			serialPacket[1] = 0x0C;
			serialPacket[2] = 8;//data length
			serialPacket[3] = bpsRawPressure >> 24;
			serialPacket[4] = bpsRawPressure >> 16;
			serialPacket[5] = bpsRawPressure >> 8;
			serialPacket[6] = bpsRawPressure & 0xFF;
			serialPacket[7] = bpsRawTemperature >> 24;
			serialPacket[8] = bpsRawTemperature >> 16;
			serialPacket[9] = bpsRawTemperature >> 8;
			serialPacket[10] = bpsRawTemperature & 0xFF;
			serialPacketLength = 11;
			checksum = calculateChecksum(serialPacket, serialPacketLength, 3);
			serialPacket[11] = checksum >> 8;
			serialPacket[12] = checksum & 0xFF;
			serialPacketLength = 13;
			//verifyChecksum(serialPacket, serialPacketLength, 3);
			break;
		default:
			break;
	}
	if (serialPacketLength != 0) {
		sendPacket(serialPacket, serialPacketLength);
		delete[] serialPacket;
	}
}

void setup() {
    Serial.begin(230400);//USB ignore baudrate? 12mbit always
	Serial1.begin(115200);//GPS
	Serial2.begin(230400);//Wireless
	Wire.begin();
    Wire.setClock(400000);
	Wire.resetBus();//Might help with stability
	Wire.setDefaultTimeout(1000);
	mpu.setup();
	bps.setup();
	mms.setup();

	mmsTimer = 0;
	mpuTimer = 0;
	bpsTimer = 0;
}

void loop() {
	//MPU - 800 hz
	if (mpuTimer >= 1250) {
		mpuTimer = mpuTimer - 1250;
		mpu.read();
		sendDataPacket(1);
	}
	//MMS - 160 hz dready
	if (mmsTimer >= 6250) {
		mmsTimer = mmsTimer - 6250;
		if (mms.isDataReady()) {
			mms.read();
			sendDataPacket(2);
		}
	}
	//BPS - 50 hz
	if (bpsTimer >= 10000) { //TODO
		bpsTimer = bpsTimer - 10000;//bpsTimer = bpsTimer - 8250;
		if (bpsState == 0) {
			bps.readD1();
			bpsState = 1;
		} else if (bpsState == 1) {
			bps.readC();
			bps.readD2();
			bpsState = 2;
		} else if (bpsState == 2) {
			bps.readC();
			bps.readD1();
			bpsState = 1;
			sendDataPacket(3);
		}
	}

	while(Serial1.available()) {
		gps.inputData(Serial1.read());
	}
	gps.processData();

	recieveData();
	processData();

	loopsPerSecond++;
	//LoopTick
	if (loopTimer >= 1000000) {
		loopTimer = loopTimer - 1000000;
		Serial2.print("Loops:");
		Serial2.print(loopsPerSecond);
		Serial2.print("\n");
		Serial2.write(gps._gpsData,128);
		loopsPerSecond = 0;
	}
}