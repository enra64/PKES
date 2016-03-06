/*
 * ADConverter.h
 *
 * Created: 11-Nov-15 13:19:05
 *  Author: Arne
 */ 


#ifndef ADCONVERTER_H_
#define ADCONVERTER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "floating_average.h"


//floating average class used for casting a floating average over the sensor values to kill spikes
//extern FloatingAverage pin1Average(5), pin2Average(5);

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

int readDistanceSensor(uint8_t sensorPin, uint8_t type);
int interpretDistanceSensor(int16_t value, uint8_t type);
void initAdc(bool freeRunning);
uint16_t readVoltageBlocking(uint8_t pin);
void readVoltageInterrupt(int pin);

#endif /* ADCONVERTER_H_ */
