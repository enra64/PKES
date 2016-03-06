/*
 * ADConverter.cpp
 *
 * Created: 11-Nov-15 13:18:51
 *  Author: Arne
 */ 
#include "ADConverter.h"

void initAdc(){
  //clear register
  ADCSRA = 0;
  //prescaler
  ADCSRA |= (1<<ADPS2);
  //enable adc
  ADCSRA |= (1<<ADEN);  
  //clear admux, also set reference to AREF
  ADMUX = 0;

  //set aref to avcc
  ADMUX |= (1 << REFS0);

  //do first conversion to get longest conversion out of the way
  ADCSRA |= (1 << ADSC);
}

uint16_t readVoltageBlocking(uint8_t pin){
  //clear admux
  ADMUX &= 0xf0;
  
  //set pin
  ADMUX |= pin;
  
	//start conversion
	ADCSRA |= (1<<ADSC);

  //clear interrupt flag
  ADCSRA |= (1 << ADIF);
  
  //while(ADCSRA & (1<<ADSC));//wait for sample end
  while(ADCSRA & (1<<ADIF));//wait for sample end

  ADCSRA |= (1 << ADIF);
  
	return ADC;//is adch, adcl
}

void readVoltageInterrupt(int pin){
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1 << ADIE);
	ADMUX = pin;
	
	//enable global interrupts
	sei();
	
	//start conversion
	ADCSRA |= (1<<ADSC);
}

ISR(ADC_vect){
  //start next conversion
	ADCSRA |= (1<<ADSC);
}
