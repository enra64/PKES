/*
 * arnes_serial.cpp
 *
 * Created: 11/8/2015 12:00:29 PM
 *  Author: Arne
 */ 
#include "arnes_serial.h"
#include "arnes_fifo_buffer.h"

	fifo_buffer Serial::txBuffer;
	fifo_buffer Serial::rxBuffer;

	/* Data available?
	 * Bit 7 in UCSR0A is set when data is available and has not yet been read;
	 */
	bool Serial::serialIsDataAvailable(){
		return !rxBuffer.isEmpty();
	}

	/* Data sent?
	 * Bit 6 in UCSR0A is set when all data has been transmitted;
	 */
	bool Serial::serialIsDataSent(){
		return (SERIAL_0_STATUS & (1 << UDRE0));
	}

	/* 
	 * Get data from serial rx buffer
	 */
	unsigned char Serial::serialRead(){
		return rxBuffer.read();
	}

	/*
	 * Send a byte via serial. blocks until sent
	 */
	void Serial::serialWrite(unsigned char dataOut){
		if(!txBuffer.isEmpty())
			txBuffer.write(dataOut);
		else
			serialDirectWrite(dataOut);
	}

	//write to the usart register. only do this with thinking, as it overrides stuff currently in there and ignores the buffer
	void Serial::serialDirectWrite(unsigned char data){
		while(!serialIsDataSent()) {}
			UDR0 = data;
	}

	/*
	 * send all given bytes via tx buffer
	 */
	void Serial::serialWrite(const char dataOut[], uint8_t size){
		//because the transmit complete interrupt only fires when a transmit has been completed, we send the first byte
		//directly, and write the rest to the buffer
		if(size == 0)
			return;
		//if the buffer is empty, we write something first to trigger the interrupts writing from buffer
		if(txBuffer.isEmpty()){
			//write first byte
			serialDirectWrite(dataOut[0]);
			//write the rest to the buffer for the interrupt send routine to do its work
			for(int i = 1; i < size; i++)
				txBuffer.write(dataOut[i]);	
		}
		else{
			for(int i = 0; i < size; i++)
				txBuffer.write(dataOut[i]);	
		}
	}

	/************************************************************************/
	/* TERMINATE the string with a NUL char                                 */
	/************************************************************************/
	void Serial::serialWriteString_0(const char* dataOut){
		serialWrite(dataOut, strlen(dataOut));
	}

	/************************************************************************/
	/* converts an integer to its ascii representation and sends it via UART*/
	/************************************************************************/
	void Serial::serialWriteDecimal(int16_t val){
		//get the number of digits
		uint8_t numberOfDigits = digitCount(val);
		//alloc space for minus sign
		if(val < 0)
			numberOfDigits++;
		//create buffer for result
		char resultString[numberOfDigits];
		//convert to string
		itoa(val, resultString, 10);
		//write to serial
		serialWriteString_0(resultString);
	}

	/************************************************************************/
	/* converts an integer to its ascii representation and sends it via UART*/
	/************************************************************************/
	void Serial::serialWriteDecimal(uint16_t val){
		//get the number of digits
		uint8_t numberOfDigits = digitCount(val);
		//create buffer for result
		char resultString[numberOfDigits];
		//convert to string
		utoa(val, resultString, 10);
		//write to serial
		serialWriteString_0(resultString);
	}

	/*
	 * Initialize serial with the help of serial utilities provided by atmel in setbaud.h
	 */
	void Serial::serialInit(){
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
	
	void Serial::writeToRxBuffer(uint8_t val){
		//e.g. read received byte from UDR0 to variable
		//char received = UDR0;
		//TODO: place byte in buffer
		//UDR0 = received;
		rxBuffer.write(val);
		//UDR0 = rxBuffer.read();
	}
	
	void Serial::sendFromTxBuffer(void){
		if(!txBuffer.isEmpty())
			UDR0 = txBuffer.read();
	}

	ISR(USART0_RX_vect){
		Serial::writeToRxBuffer(UDR0);
	}

	ISR(USART0_TX_vect){
		Serial::sendFromTxBuffer();
	}

	/*
	 * this does not disable interrupts if false
	 */
	void Serial::serialEnableInterrupts(bool rx, bool tx){
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