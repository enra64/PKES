/*
 * library.h
 *
 * Created: 16-Oct-15 11:36:28
 *  Author: Arne
 */ 

#ifndef LIBRARY_H_
#define LIBRARY_H_

//processor speed for delay.h
#define F_CPU 16000000L

//serial registers
#define SERIAL_0_STATUS UCSR0A
#define SERIAL_0_CONFIG_1 UCSR0B
#define SERIAL_0_CONFIG_2 UCSR0C
//serial values
#define BAUD 57600
#define SERIAL_BUFFER_SIZE 64

//includes
#include <stdbool.h>//bool  
#include <avr/io.h>//uint8_t
#include <avr/interrupt.h>//enable interrupts, currently mainly for serial communication

//struct defines
struct fixed_point_number
{
	int number;
	uint8_t commaPos;
};

//seven segment display pins
#define SEVEN_SEGMENT_CLOCK PC2
#define SEVEN_SEGMENT_DATA PC3
#define SEVEN_SEGMENT_DATA_ENABLE PC4

//led pins
#define LED_1 PJ1
#define LED_2 PA5
#define LED_3 PA4
#define LED_4 PD3

const uint8_t SEVEN_SEGMENT_NUMBERS[] = {
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

const uint8_t SEVEN_SEGMENT_ALPHABET[] = {
	0b11101110,//A0
	0b00111110,//b1
	0b10011100,//C2
	0b00011010,//c3
	0b01111010,//d4
	0b10011110,//E5
	0b10001110,//F6
	0b01101110,//H7
	0b00101110,//h8
	0b00011100,//L9
	0b01100000,//I10
	0b11111100,//O11
	0b00111010,//o12
	0b11001110,//P13
	0b10110110,//S14
	0b00000000 //whitespace15
};

//7SD function prototypes
void writeDigital(volatile uint8_t* port, int pin, bool val);
void send7SDBit(bool val);
void send7SDTransmission(unsigned char dataBytes[]);

//serial function prototypes
bool isSerialDataAvailable_0(void);
bool isSerialDataSent_0(void);
unsigned char readSerial_0(void);
void writeSerial_0(unsigned char dataOut);
void initSerial_0(void);
//serial interrupts
void enableSerialInterrupts_0(void);

#endif /* LIBRARY_H_ */