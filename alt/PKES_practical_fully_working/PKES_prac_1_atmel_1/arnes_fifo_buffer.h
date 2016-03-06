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

class fifo_buffer{
	uint8_t _data[SERIAL_BUFFER_SIZE];
	unsigned int _head, _tail, _size;
	
	public:
	bool isEmpty(void){
		return _head == _tail;
	}
	bool isFull(void){
		return _size == SERIAL_BUFFER_SIZE;
	}
	bool write(uint8_t newValue){
		if(isFull())
			return false;
		_size++;
		_data[_head++] = newValue;
		return true;
	}
	uint8_t read(void){
		if(isEmpty())
			return 0x00;
		_size--;
		return _data[_tail++];
	}
	void clear(void){
		_size = 0;
		_head = 0;
		_tail = 0;
	}
};
#endif