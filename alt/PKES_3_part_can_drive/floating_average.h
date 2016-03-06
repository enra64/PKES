/*
 * arnes_fifo_buffer.cpp
 *
 * Created: 22-Oct-15 14:36:21
 *  Author: Arne
 */ 
#ifndef flav
#define flav

//generic includes
#include <stdbool.h>//bool
#include <avr/io.h>//uint8_t

class FloatingAverage{
  int16_t* _buffer;
  uint8_t _bufferSize, _head = 0;
  
	public:
  FloatingAverage(){
    uint8_t _size = 16;
    _bufferSize = _size;
    _buffer = new int[_size];
  }
  //init the buffer, save its size
  FloatingAverage(uint8_t _size){
    _bufferSize = _size;
    _buffer = new int[_size];
  }

  //destructor to avoid leaking the buffer
  ~FloatingAverage(){
    delete[] _buffer;
  }

  //add a new value, removing the oldest
	bool write(int16_t newValue){
    _buffer[_head] = newValue;
    _head = (_head + 1) % _bufferSize;
	}

  int16_t* getBuffer(void){
    return _buffer;
  }
  
  //get the average of all contained values. 
	int16_t readAvg(void){
    int32_t sum = 0;
    for(uint8_t i = 0; i < _bufferSize; i++)
      sum += _buffer[i];
    return sum / _bufferSize;
	}
 
	void clear(void){
		for(uint8_t i = 0; i < _bufferSize; i++)
      _buffer[i] = 0;
		_head = 0;
	}
};
#endif
