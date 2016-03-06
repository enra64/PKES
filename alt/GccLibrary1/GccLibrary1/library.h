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

//pins
#define SEVEN_SEGMENT_CLOCK PC2
#define SEVEN_SEGMENT_DATA PC3
#define SEVEN_SEGMENT_DATA_ENABLE PC4

//function prototypes
void writeDigital(int port, int pin, bool val);
void send7SDBit(bool val);
void send7SDTransmission(unsigned char dataBytes[]);

#endif /* LIBRARY_H_ */