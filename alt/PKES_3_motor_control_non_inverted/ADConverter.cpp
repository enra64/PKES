/*
 * ADConverter.cpp
 *
 * Created: 11-Nov-15 13:18:51
 *  Author: Arne
 */ 
#include "ADConverter.h"


void initAdc(bool freeRunning){
  //disable global interrupts
  sei();
  //clear register
  ADCSRA = 0;
  //prescaler
  ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // 125kHz
  //enable adc
  ADCSRA |= (1<<ADEN);
  
  //clear admux, also set reference to AREF
  ADMUX = 0;
  
  //set aref to avcc
  ADMUX |= (1 << REFS0);
  
  if(freeRunning){
    //enable adate for free running
    ADCSRA |= (1<<ADATE) | (1 << ADIE);
    
    //free running
    ADCSRB &= ~ (1 << ADTS0) | (1 << ADTS1) | (1 << ADTS2);
  }
  
  //do first conversion to get longest conversion out of the way
  ADCSRA |= (1 << ADSC);
  
  //enable global interrupts
  sei();
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

int readDistanceSensor(uint8_t sensorPin, uint8_t type){
  return interpretDistanceSensor(readVoltageBlocking(sensorPin), type);
}

//type must be 49(0), 39(2) or 47(1)
int interpretDistanceSensor(int16_t value, uint8_t type){
  if(type == 49 || type == 0)
    for(int i = 0; i < 46; i+=2)
      if(value > CONV_TBL_49[i])
        return CONV_TBL_49[i+1];
  else if(type == 47 || type == 1)
    for(int i = 0; i < 46; i+=2)
      if(value > CONV_TBL_47[i])
        return CONV_TBL_47[i+1];
  else if(type == 39 || type == 0)
    for(int i = 0; i < 46; i+=2)
      if(value > CONV_TBL_39[i])
        return CONV_TBL_39[i+1];
  else
    return -1;
  return 999;
}

void readVoltageInterrupt(int pin){
  //clear interrupt flag, enable adc, set prescaler, enable interrupts
	ADCSRA = (1 << ADIF) | (1<<ADEN) | (1<<ADPS2) | (1 << ADIE);

  //clear admux, set pin
  ADMUX &= 0xf0;
	ADMUX |= pin;
	
	//start conversion
	ADCSRA |= (1<<ADSC);
}
