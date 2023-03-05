#ifndef CircularBuffer_hpp
#define CircularBuffer_hpp

class CircularBuffer {
    public:
        CircularBuffer();
        void inputData(unsigned char data);
        unsigned char getData(unsigned char position);
        void trimData(unsigned char amount);
        bool isFull();
        bool isEmpty();
        unsigned char getFreeCapacity();
        unsigned char dataLength();
        unsigned char totalCapacity();
    private:
        unsigned char _data[128];
        unsigned char _head = 0;
        unsigned char _tail = 0;
        unsigned char _maxSize = 0;
        unsigned char _dataLength = 0;
};

#endif