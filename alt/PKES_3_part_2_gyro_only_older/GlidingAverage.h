#ifndef GL_AVG
#define GL_AVG

//generic includes
#include <stdbool.h>//bool
#include <avr/io.h>//uint8_t

#define GL_AVG_SIZE 32

class GlidingAverage{
  int16_t _buffer[GL_AVG_SIZE];
  uint8_t _head = 0;
  
	public:
  //add a new value, removing the oldest
	bool write(int16_t newValue){
    _buffer[_head] = newValue;
    _head = (_head + 1) % GL_AVG_SIZE;
	}

  int16_t* getBuffer(void){
    return _buffer;
  }
  
  //get the average of all contained values. 
	int32_t readAvg(void){
    int32_t sum = 0;
    for(uint8_t i = 0; i < GL_AVG_SIZE; i++)
      sum = sum + _buffer[i];
    return sum / GL_AVG_SIZE;
	}
 
	void clear(void){
		for(uint8_t i = 0; i < GL_AVG_SIZE; i++)
      _buffer[i] = 0;
		_head = 0;
	}
};
#endif
