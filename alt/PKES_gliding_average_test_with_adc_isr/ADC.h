#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <avr/interrupt.h>

const uint16_t CONV_TBL_47[]{
  587,3,
  550,4,
  451,5,
  388,6,
  338,7,
  305,8,
  273,9,
  244,10,
  225,11,
  209,12,
  195,13,
  184,14,
  172,15,
  160,16,
  152,17,
  144,18,
  140,19,
  137,20,
  129,21,
  124,22,
  120,23,
  116,24,
  110,25};

const uint16_t CONV_TBL_39[]{
  572,3,
  536,4,
  455,5,
  394,6,
  340,7,
  302,8,
  270,9,
  248,10,
  227,11,
  209,12,
  194,13,
  180,14,
  175,15,
  165,16,
  156,17,
  148,18,
  143,19,
  137,20,
  125,21,
  117,22,
  108,23,
  100,24,
  95,25
};

const uint16_t CONV_TBL_49[] = {//nelson... 49
  600,3,
  500,4,
  444,5,
  380,6,
  335,7,
  300,8,
  275,9,
  250,10,
  240,11,
  218,12,
  205,13,
  190,14,
  180,15,
  172,16,
  163,17,
  156,18,
  150,19,
  143,20,
  142,21,
  141,22,
  140,23,
  135,24,
  130,25};



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
    ADCSRA |= (1<<ADATE);

    //enable interrupt for notification
    ADCSRA |= (1 << ADIE);
    
    //free running
    ADCSRB &= ~((1 << ADTS0) | (1 << ADTS1) | (1 << ADTS2));
  }
  
  //do first conversion to get longest conversion out of the way
  ADCSRA |= (1 << ADSC);
  
  //enable global interrupts
  sei();
}

void setAdmux(uint8_t pin){
  //clear admux
  ADMUX &= 0xf0;
  
  //set pin
  ADMUX |= pin;
}

uint16_t readVoltageBlocking(uint8_t pin){
  setAdmux(pin);
  
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

void readVoltageInterrupt(int pin){
  //clear interrupt flag
  ADCSRA = (1 << ADIF);

  //clear admux, set pin
  ADMUX &= 0xf0;
  ADMUX |= pin;
  
  //start conversion
  ADCSRA |= (1<<ADSC);
}

#endif /* ADCONVERTER_H_ */
