/*
 * arnes_7SD.h
 *
 * Created: 11/8/2015 12:02:13 PM
 *  Author: Arne
 */ 


#ifndef ARNES_7SD_H_
#define ARNES_7SD_H_

//generic includes
#include "arnes_library.h"
#include <stdbool.h>//bool
#include <avr/io.h>//uint8_t
#include <util/delay.h>

//seven segment display pins
#define SEVEN_SEGMENT_CLOCK PC2
#define SEVEN_SEGMENT_DATA PC3
#define SEVEN_SEGMENT_DATA_ENABLE PC4

const uint8_t NUMBERS_7SD[] = {
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

class SevenSegmentDisplay{
	public:
	//7SD function prototypes
	static void send7SDBit(bool val);
	static void send7SDTransmission(unsigned char dataBytes[]);
	static void displayNumber(double val);
	static void displayNumber(int val);
	static void initialise(void);
};
#endif /* ARNES_7SD_H_ */