/*
 * ADConverter.cpp
 *
 * Created: 11-Nov-15 13:18:51
 *  Author: Arne
 */ 
#include "ADConverter.h"

uint16_t ADConverter::result = 0;

uint16_t ADConverter::readVoltageBlocking(int pin){
	ADCSRA = (1<<ADEN) | (1<<ADPS2);
	ADMUX = pin;
	
	//start conversion
	ADCSRA |= (1<<ADSC);
	
	while(ADCSRA & (1<<ADSC)){//wait for sample end
	}
	
	return ADCW;//is adch, adcl
}

void ADConverter::readVoltageInterrupt(int pin){
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1 << ADIE);
	ADMUX = pin;
	
	//enable global interrupts
	sei();
	
	//start conversion
	ADCSRA |= (1<<ADSC);
}

ISR(ADC_vect){
	ADConverter::result = ADC;
	//start next conversion
	ADCSRA |= (1<<ADSC);
}