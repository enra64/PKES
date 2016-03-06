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
#include <string.h>
#include <stdbool.h>

/************************************************************************/
/* begin i2c/TWI section                                                */
/************************************************************************/

void initializeTWI(void){
	//SCL frequency is F_CPU / 16 + 2 * TWBR * Prescaler
	//16 000 000 / 16 + 2 * 12 * 1 = 400 000 Hz
	TWSR = 0x00;//set prescaler to 1
	TWBR = 0b00001100;//TWI Bit Rate Register
	//enable TWI
	TWCR = (1<<TWEN);
}

void sendTWIStart(void){
	//generate a start condition as per datasheet (reset interrupt flag)
	TWCR = (1 << TWINT) | (1<<TWSTA) | (1<<TWEN);
	//wait until interrupt flag is cleared -> start condition is ok
	while((TWCR & (1<<TWINT)) == 0);
}

void sendTwiStop(void){
	TWCR = (1 << TWINT) | (1<<TWSTO) | (1<<TWEN);
}

void sendTwiData(uint8_t val){
	TWDR = val;
	TWCR = (1<<TWINT) | (1<<TWEN);
	//wait for transmission end
	while((TWCR & (1<<TWINT)) == 0);
}

uint8_t readTwiACK(void){
	//magic
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	//wait until data has been received
	while ((TWCR & (1<<TWINT)) == 0);
	//return data
	return TWDR;
}

uint8_t readTwiStatus(void){
	uint8_t status;
	//remove lower three bits as
	status = TWSR & 0xF8;
	return status;
}

/************************************************************************/
/* end i2c/TWI section                                                */
/************************************************************************/

/************************************************************************/
/* begin serial 0 control section                                       */
/************************************************************************/

fifo_buffer rxBuffer, txBuffer;

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
 * Get data from serial rx buffer
 */
unsigned char readSerial_0(){
	return rxBuffer.read();
}

/*
 * Send a byte via serial. blocks until sent
 */
void writeSerial_0(unsigned char dataOut){
	while(!isSerialDataSent_0()) {}
	UDR0 = dataOut;
}

/*
 * send all given bytes via tx buffer
 */
void writeSerial_0(const char dataOut[], uint8_t size){
	//because the transmit complete interrupt only fires when a transmit has been completed, we send the first byte
	//directly, and write the rest to the buffer
	if(size == 0)
		return;	
	//write first byte
	writeSerial_0(dataOut[0]);
	//write the rest to the buffer for the interrupt send routine to do its work
	for(int i = 1; i < size; i++)
		txBuffer.write(dataOut[i]);
}

/************************************************************************/
/* TERMINATE the string with a NUL char                                 */
/************************************************************************/
void writeSerialString_0(const char* dataOut){
	writeSerial_0(dataOut, strlen(dataOut));
}

/************************************************************************/
/* converts an integer to its ascii representation and sends it via UART*/
/************************************************************************/
void writeSerialDecimal_0(int val){
	//get the number of digits
	uint8_t numberOfDigits = digitCount(val);
	//alloc space for minus sign
	if(val < 0)
		numberOfDigits++;
	//alloc
	char resultString[numberOfDigits];
	//save digits as ascii char in array
	//for(int i = 0; i < numberOfDigits; i++)
	for(int i = numberOfDigits - 1; i >= 0; i--)
		resultString[i] = (int)(val / pow(10, i)) % 10 + 48;//get digit, convert to ascii
	//add a minus
	if(val < 0)
		resultString[numberOfDigits - 1] = '-';
	//write to serial
	writeSerial_0(resultString, numberOfDigits);
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
	
	//reset both config bytes, even though it does not make sense
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

ISR(USART0_TX_vect){
	if(!txBuffer.isEmpty())
		UDR0 = txBuffer.read();
}

/*
 * this does not disable interrupts if false
 */
void enableSerialInterrupts_0(bool rx, bool tx){
	//enable serial receive interrupt
	if(rx)
		UCSR0B |= (1 << RXCIE0);
	//enable serial transmit interrupt
	if(tx){
		UCSR0B |= (1 << TXCIE0);
		UDR0 = 0;
	}
	//enable global interrupt flag
	if(tx || rx)
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
 * writeDigital(&PORTC, PC2, true)
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

/************************************************************************/
/* dumb shit copied from stackoverflow and tiny helper functions        */
/************************************************************************/
//fastest way to do it btw
int digitCount (int n) {
	//lets hope n is != minint, because in c, that fucks up everything
    if (n < 0) n = -n;
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
    /*      2147483647 is 2^31-1 - add more ifs as needed
       and adjust this final return as well. */
    return 10;
}
