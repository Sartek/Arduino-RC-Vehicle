#ifndef PacketHandler_hpp
#define PacketHandler_hpp

void sendPacket(char packet[], unsigned int packetLength);
unsigned int calculateChecksum(char packet[], unsigned int packetLength, unsigned int headerOffset);
bool verifyChecksum(char packet[], unsigned int packetLength, unsigned int headerOffset);
void recieveData();
void processData();

#endif