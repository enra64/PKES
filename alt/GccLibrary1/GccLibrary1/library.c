/*
 * GccLibrary1.c
 *
 * Created: 16-Oct-15 09:34:15
 * Author : Arne
 */

#include "library.h"
#include <avr/io.h>
#include <util/delay.h> 
#include <stdbool.h>


uint8_t SEVEN_SEGMENT_NUMBERS[] = {
	0b11111100,
	0b01100000,
	0b11011010,
	0b11110010,
	0b01100110,
	0b10110110,
	0b10111110,
	0b11100000,
	0b11111110,
	0b11110110,
	0b00000010};

uint8_t SEVEN_SEGMENT_ALPHABET[] = {
	0b11101110,//A
	0b00111110,//b
	0b10011100,//C
	0b00011010,//c
	0b01111010,//d
	0b10011110,//E
	0b10001110,//F
	0b01101110,//H
	0b00101110,//h
	0b00011100,//L
	0b01100000,//I
	0b11111100,//O
	0b00111010,//o
	0b11001110,//P
	0b10110110//S
};

/*
 * Write high or low to a pin on a port
 */
void writeDigital(int port, int pin, bool val){
	if(val)
		port |= (1<<pin);//set bit
	else
		port &= ~ (1<<pin);//delete bit
}

/*
* Writes a single data bit to the seven segment display
*/
void send7SDBit(bool val){
	//pull data bit
	writeDigital(PORTC, SEVEN_SEGMENT_DATA, val);
	_delay_us(200);
	//pull clock high, low to read in data bit
	writeDigital(PORTC, SEVEN_SEGMENT_CLOCK, 1);
	_delay_us(200);
	writeDigital(PORTC, SEVEN_SEGMENT_CLOCK, 0);
}

/************************************************************************/
/* Send three data bytes to the display while							*/													
/* complying to the data sheet requirements of the display              */
/************************************************************************/
void send7SDTransmission(unsigned char dataBytes[]){
		//enable write
		writeDigital(PORTC, SEVEN_SEGMENT_DATA_ENABLE, 0);
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
		writeDigital(PORTC, SEVEN_SEGMENT_DATA_ENABLE, 1);
}