/*
 * arnes_twi.h
 *
 * Created: 11/8/2015 12:28:12 PM
 *  Author: Arne
 */ 


#ifndef ARNES_TWI_H_
#define ARNES_TWI_H_

#include <util/twi.h>
#include <avr/io.h>
#include "arnes_library.h"

class Twi{
	public:
	//twi function prototypes
	static void twiInitialize(void);
	static bool twiStart(void);
	static bool twiStart(bool repeatedStart);
	static void twiStop(void);
	static void twiWriteByte(uint8_t val);
	static void twiReadAck(uint8_t *data);
	static uint8_t twiWriteTo(uint8_t address, uint8_t data, bool repeated, bool stop);
	static uint8_t twiReadFrom(uint8_t address, uint8_t* data, bool repeated, bool stop);
};



#endif /* ARNES_TWI_H_ */