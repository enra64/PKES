/*
 * arnes_fifo_buffer.cpp
 *
 * Created: 22-Oct-15 14:36:21
 *  Author: Arne
 */ 
#ifndef arnes_fifo_buffer
#define arnes_fifo_buffer

//generic includes
#include <stdbool.h>//bool
#include <avr/io.h>//uint8_t

class Queue{
	uint8_t _data[buffer_size];
	unsigned int _head, _tail, _size;
  unsigned uint8_t buffer_size;
	
	public:
  Queue(uint8_t _size){
    buffer_size = _size;     
  }
  
	bool isEmpty(void){
		return _head == _tail && _size == 0;
	}
 
	bool isFull(void){
		return _size == buffer_size;
	}
 
	bool write(uint8_t newValue){
		if(isFull())
			return false;
		_size++;
		_data[(_head % buffer_size)] = newValue;
   _head = (_head + 1) % buffer_size;
		return true;
	}
	
	uint8_t read(void){
		if(isEmpty())
			return 0x00;
		_size--;
    uint8_t res = _data[_tail % buffer_size];
    _tail = (_tail + 1) % buffer_size;
		return res;
	}
 
	void clear(void){
		_size = 0;
		_head = 0;
		_tail = 0;
	}
};
#endif
