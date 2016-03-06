/*
 * arnes_twi.cpp
 *
 * Created: 11/8/2015 12:28:26 PM
 *  Author: Arne
 */ 

#include "arnes_twi.h"


void Twi::twiInitialize(void){
	//SCL frequency is F_CPU / 16 + 2 * TWBR * Prescaler
	//16 000 000 / 16 + 2 * 12 * 1 = 400 000 Hz
	TWSR = 0x00;//set prescaler to 1
	TWBR = 0b00001100;//TWI Bit Rate Register for 400k
	//TWBR = 0b01001000;//TWI Bit Rate Register for 100k
	//enable TWI
	TWCR = (1<<TWEN);
}

/*
 * Desc		Create a non-repeated start condition on TWI bus and wait until the hardware is finished.
 * Output	true: start condition has been successfully sent
 *			false: not so much
 */
bool Twi::twiStart(void){
	return twiStart(false);
}

/*
 * Desc		Create a start condition on TWI bus and wait until the hardware is finished.
 * Input	Check for a repeated start success or a normal start success
 * Output	true: start condition has been successfully sent
 *			false: not so much
 */
bool Twi::twiStart(bool repeatedStart){
		//generate a start condition as per datasheet (reset interrupt flag)
		TWCR = (1 << TWINT) | (1<<TWSTA) | (1<<TWEN);
		//wait until interrupt flag is cleared -> start condition is ok
		while((TWCR & (1<<TWINT)) == 0);
		//if this is a repeated start, check for that condition
		if(repeatedStart)
			return TW_STATUS == TW_REP_START;
		//check normal start condition
		return TW_STATUS == TW_START;
}

void Twi::twiStop(void){
	TWCR = (1 << TWINT) | (1<<TWSTO) | (1<<TWEN);
}

void Twi::twiWriteByte(uint8_t val){
	TWDR = val;
	TWCR = (1<<TWINT) | (1<<TWEN);
	//wait for transmission end
	while((TWCR & (1<<TWINT)) == 0);
}

void Twi::twiReadAck(uint8_t *data){
	//magic//enable interrupts, enable twi, acknowledge
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	//wait until data has been received
	while ((TWCR & (1<<TWINT)) == 0);
	//return data
	*data = TWDR;
}

/*
 * Desc		Create a START condition, send slave address, send data byte, send stop
 *			ALWAYS BEGIN with start, then ALWAYS an address, then data.
 * Input	repeated: whether we are in a repeated phase
 * Output	0: success
 *			1: start error
 *			2: address send error
 *			3: data send error
 */
uint8_t Twi::twiWriteTo(uint8_t address, uint8_t data, bool repeated, bool stop){
	//send START condition, abort if error
	if(!twiStart(repeated))
		return 1;
		
	//send ADDRESS
	//shift the address to the left and add write bit, send.
	twiWriteByte((address << 1) + TW_WRITE);
	//check for address send fuck-up
	if(TW_STATUS != TW_MT_SLA_ACK)
		return 2;
		
	//send first DATA byte
	twiWriteByte(data);
	//check for data send fuck-up
	if(TW_STATUS != TW_MT_DATA_ACK)
		return 3;
	
	if(stop)
		twiStop();
		
	return 0;
}

uint8_t Twi::twiReadFrom(uint8_t address, uint8_t* data, bool repeated, bool stop){
	//send START condition, abort if error
	if(!twiStart(repeated))
		return 1;
	
	//send ADDRESS
	//shift the address to the left and add write bit, send.
	twiWriteByte((address << 1) + TW_WRITE);
	//check for address send fuck-up
	if(TW_STATUS != TW_MT_SLA_ACK)
		return 2;
	
	twiReadAck(data);
	
	if(stop)
		twiStop();
	
	return 0;
}