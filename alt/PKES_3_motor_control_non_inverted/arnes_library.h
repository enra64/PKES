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

//includes
#include <stdbool.h>//bool  
#include <avr/io.h>//uint8_t
#include <avr/interrupt.h>//enable interrupts, currently mainly for serial communication

//struct defines
struct imu_data{
	int16_t x, y, z;
};

//led pins
#define LED_1 PJ1
#define LED_2 PA5
#define LED_3 PA4
#define LED_4 PD3

//general function prototypes
void writeDigital(volatile uint8_t* port, int pin, bool val);
int digitCount(int val);
uint8_t getDigit(double val, int pos);
#endif /* LIBRARY_H_ */
