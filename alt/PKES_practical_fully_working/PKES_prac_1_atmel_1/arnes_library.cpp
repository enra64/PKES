/*
 * arnes_library.cpp
 * Created: 16-Oct-15 09:34:15
 * Author : Arne
 */

#include "arnes_library.h"
#include "arnes_fifo_buffer.h"
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <util/twi.h>
#include <stdbool.h>

/************************************************************************/
/* begin i2c/TWI section                                                */
/************************************************************************/

void initializeTWI(void){
	
}

/************************************************************************/
/* end i2c/TWI section                                                */
/************************************************************************/

/************************************************************************/
/* begin serial 0 control section                                       */
/************************************************************************/

fifo_buffer rxBuffer;

/* Data available?
 * Bit 7 in UCSR0A is set when data is available and has not yet been read;
 */
bool isSerialDataAvailable_0(){
	return !rxBuffer.isEmpty();
	//return (SERIAL_0_STATUS & (1 << RXC0));//probably could remove comparison since bool just defines 0, 1 as false, true
}

/* Data sent?
 * Bit 6 in UCSR0A is set when all data has been transmitted;
 */
bool isSerialDataSent_0(){
	return (SERIAL_0_STATUS & (1 << UDRE0));
}

/* 
 * Get data from serial. blocks until received
 */
unsigned char readSerial_0(){
	return rxBuffer.read();
	//wait till data is available
	//while(!isSerialDataAvailable_0()) {}
	//return UDR0;
}

/*
 * Send a byte via serial. blocks until sent
 */
void writeSerial_0(unsigned char dataOut){
	while(!isSerialDataSent_0()) {}
	UDR0 = dataOut;
}

/*
 * Initialize serial with the help of serial utilities provided by atmel in setbaud.h
 */
void initSerial_0(){
	//calculated by setbaud.h, which also checks for baud rate range and possible double speed mode
	//formula is (F_CPU/16/BAUD)-1 per data sheet
	UBRR0H = UBRRH_VALUE;//first byte into 
	UBRR0L = UBRRL_VALUE;//shift re
	
	//double baud rate?
	#if USE_2X
		UCSR0A |= (1 << U2X0);
	#else
		UCSR0A &= ~(1 << U2X0);
	#endif
	
	//reset both config bytes, even though it should not be necessary
	UCSR0C = 0x00;
	UCSR0B = 0x00;
	
	//set 8 bits of data, no parity seems to be default
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
	
	//enable RX, TX systems
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	
	/*concluded info: 
	UCSR0A: mostly status register
	RXC0-receive complete?
	TXC0-transmit complete
	UDRE0-is data register (UDR0) empty, e.g. can we transmit or receive new data
	FE0-frame error
	DOR0-data overrun - UDR0 was not read in time, so the data was overwritten
	UPE0-frame in UDR0 has parity error
	U2X0-double transmission speed?
	MPCM0-ignore data if no address is provided
	
	UCSR0B:000 11 0 00
	RXCIE0-enable interrupts for receive complete
	TXCIE0-enable interrupts for transmit complete
	UDRIE0-enable interrupts for UDR0 read
	RXEN0-enable receiver
	TXEN0-enable transmitter
	UCSZ20-set for 9 bit frame size
	RXB80-8th received bit
	TXB80-8th sent bit
	
	UCSR0C:00 00 0 110
	UMSEL00-set op mode, 00 for asynchronous
	UMSEL01- "
	UPM00-parity mode - 00 for none
	UPM01- "
	USBS0-1 stop bit when not set, 2 when set
	UCSZ00-set both for 8 bit frames
	UCSZ01- "
	UCPOL0-clock polarity (e.g. transmit on falling/rising clock edge), only relevant for sync operation
	*/
}

ISR(USART0_RX_vect){
	//eg read received byte from UDR0 to variable
	//char received = UDR0;
	//TODO: place byte in buffer
	//UDR0 = received;
	rxBuffer.write(UDR0);
	//UDR0 = rxBuffer.read();
}

void enableSerialInterrupts_0(){
	//enable serial receive interrupt
	UCSR0B |= (1 << RXCIE0);
	//enable global interrupt flag
	sei();
}

/************************************************************************/
/* end serial 0 section                                                 */
/************************************************************************/


/************************************************************************/
/* begin seven segment display section                                  */
/************************************************************************/

/*
 * Write high or low to a pin on a port
 * writeDigital(PORTC, PC2, true)
 */
void writeDigital(volatile uint8_t* port, int pin, bool val){
	if(val)
		*port |=  (1<<pin);//set bit
	else
		*port &=~ (1<<pin);//delete bit
}

/*
* Writes a single data bit to the seven segment display
*/
void send7SDBit(bool val){
	//pull data bit
	writeDigital(&PORTC, SEVEN_SEGMENT_DATA, val);
	_delay_us(200);
	//pull clock high, low to read in data bit
	writeDigital(&PORTC, SEVEN_SEGMENT_CLOCK, 1);
	_delay_us(200);
	writeDigital(&PORTC, SEVEN_SEGMENT_CLOCK, 0);
}

/************************************************************************/
/* Send three data bytes to the display while							*/													
/* complying to the data sheet requirements of the display              */
/************************************************************************/
void send7SDTransmission(unsigned char dataBytes[]){
	//enable write
	writeDigital(&PORTC, SEVEN_SEGMENT_DATA_ENABLE, 0);
	_delay_us(1);
	/*
	* BEGIN DATA ARRAY WRITE
	*/
	//per data sheet, send a 1 first
	send7SDBit(1);
	//send all three data bytes
	for(uint8_t digitPosition = 0; digitPosition < 3; digitPosition++){
		//sends a single digit
		for(int i = 7; i >= 0; i--){
			//high or low via bit shifting a mask
			bool val = (dataBytes[digitPosition] & (1 << i)) != 0;
			send7SDBit(val);
		}
	}
	//send disregarded last 12 bits
	for(int i = 0; i < 12; i++)
		send7SDBit(0);
	/*
	* END DATA ARRAY WRITE
	*/
	//"write to display"
	writeDigital(&PORTC, SEVEN_SEGMENT_DATA_ENABLE, 1);
}

/************************************************************************/
/* end seven segment display section                                    */
/************************************************************************/