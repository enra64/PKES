/*
 * arnes_serial.h
 *
 * Created: 11/8/2015 11:52:09 AM
 *  Author: Arne
 */ 


#ifndef ARNES_SERIAL_H_
#define ARNES_SERIAL_H_

//serial values
#define BAUD 57600

//generic includes
#include "arnes_library.h"
#include "arnes_fifo_buffer.h"
#include <string.h>
#include <stdlib.h>
#include <util/setbaud.h>
#include <stdbool.h>//bool
#include <avr/io.h>//uint8_t
#include <util/delay.h>

//serial registers
#define SERIAL_0_STATUS UCSR0A
#define SERIAL_0_CONFIG_1 UCSR0B
#define SERIAL_0_CONFIG_2 UCSR0C

class Serial{
		static void serialDirectWrite(unsigned char data);
	public:
		static fifo_buffer rxBuffer;
		static fifo_buffer txBuffer;
	
		static bool serialIsDataAvailable(void);
		static bool serialIsDataSent(void);
		static unsigned char serialRead(void);
		static void serialWrite(unsigned char dataOut);
		static void serialWrite(const char dataOut[], uint8_t size);
		static void serialWriteString_0(const char dataOut[]);
		static void serialInit(void);
		static void serialWriteDecimal(int val);
		//serial interrupts
		static void serialEnableInterrupts(bool rx, bool tx);
		static void serialWriteDecimal(uint16_t val);
		//functions for isr usage
		static void writeToRxBuffer(uint8_t val);
		static void sendFromTxBuffer(void);
};
#endif /* ARNES_SERIAL_H_ */