/*
 * ADConverter.h
 *
 * Created: 11-Nov-15 13:19:05
 *  Author: Arne
 */ 


#ifndef ADCONVERTER_H_
#define ADCONVERTER_H_

#include <avr/io.h>
#include <avr/interrupt.h>//enable interrupts, currently mainly for serial communication

void initAdc(void);
uint16_t readVoltageBlocking(uint8_t pin);
void readVoltageInterrupt(int pin);

#endif /* ADCONVERTER_H_ */
