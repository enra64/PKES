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

#define SIZE 3

class AverageRing{
  float _buffer[SIZE];
  uint8_t _head = 0;

public:
  bool write(float newValue){
    _buffer[_head] = newValue;
    _head = (_head + 1) % SIZE;
  }
  
  float readAvg(void){
    float sum = 0;
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
