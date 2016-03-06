#ifndef GL_AVG
#define GL_AVG

//generic includes
#include <stdbool.h>//bool
#include <avr/io.h>//uint8_t

#define SIZE 16

class GlidingAverage{
  int16_t _buffer[SIZE];
  uint8_t _head = 0;
  
	public:
  //add a new value, removing the oldest
	bool write(int16_t newValue){
    _buffer[_head] = newValue;
    _head = (_head + 1) % SIZE;
	}

  int16_t* getBuffer(void){
    return _buffer;
  }
  
  //get the average of all contained values. 
	long readAvg(void){
    long sum = 0;
    for(uint8_t i = 0; i < SIZE; i++)
      sum = sum + _buffer[i];
    return sum / SIZE;
	}
 
	void clear(void){
		for(uint8_t i = 0; i < SIZE; i++)
      _buffer[i] = 0;
		_head = 0;
	}
};
#endif
