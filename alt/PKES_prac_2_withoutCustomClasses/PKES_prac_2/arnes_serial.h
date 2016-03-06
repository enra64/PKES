/*
 * arnes_serial.h
 *
 * Created: 11/8/2015 11:52:09 AM
 *  Author: Arne
 */ 


#ifndef ARNES_SERIAL_H_
#define ARNES_SERIAL_H_

//generic includes
#include <stdbool.h>//bool
#include <avr/io.h>//uint8_t

class Serial{
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
		_data[(_head++ % SERIAL_BUFFER_SIZE)] = newValue;
		return true;
	}
	uint8_t read(void){
		if(isEmpty())
		return 0x00;
		_size--;
		return _data[_tail++ % SERIAL_BUFFER_SIZE];
	}
	void clear(void){
		_size = 0;
		_head = 0;
		_tail = 0;
	}
};
#endif /* ARNES_SERIAL_H_ */