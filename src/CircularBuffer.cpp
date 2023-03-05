#include "CircularBuffer.hpp"

CircularBuffer::CircularBuffer() {
    _maxSize = sizeof(_data);
    for(unsigned char i = 0; i < _maxSize; i = i + 1) {
        _data[i] = 0x00;
    }
}

void CircularBuffer::inputData(unsigned char data) {
    _data[_head] = data;
    _head = (_head + 1) % _maxSize;

    if (_dataLength < _maxSize) {
        _dataLength++;
    } else {
        _tail = (_tail + 1) % _maxSize;
    }
}

unsigned char CircularBuffer::getData(unsigned char position) {
    if (position < _dataLength) {
        return _data[(_tail + position) % _maxSize];
    } else {
        return 0x00;
    }
}

void CircularBuffer::trimData(unsigned char amount) {
    _tail = (_tail + amount) % _maxSize;
    if(_dataLength <= amount) {
        _tail = 0;
        _head = 0;
        _dataLength = 0;
    } else {
            _dataLength = _dataLength - amount;
    }
}

bool CircularBuffer::isFull() {
    if (_dataLength == _maxSize) {
        return true;
    } else {
        return false;
    }
}

bool CircularBuffer::isEmpty() {
    if (_dataLength == 0) {
        return true;
    } else {
        return false;
    }
}

unsigned char CircularBuffer::getFreeCapacity() {
    return _maxSize - _dataLength;
}

unsigned char CircularBuffer::dataLength() {
    return _dataLength;
}

unsigned char CircularBuffer::totalCapacity() {
    return _maxSize;
}