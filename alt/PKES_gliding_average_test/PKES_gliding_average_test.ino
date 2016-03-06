#include "GlidingAverage.h"
#include "Motor.h"
#include "Gyroscope.h"
#include "ADC.h"
#include "General.h"
#include "SSD.h"
#include "Imu.h"
#include <Wire.h>
#include <avr/io.h>
#include <avr/interrupt.h>

GlidingAverage _avg0, _avg1;
Gyro _gyro;
uint8_t _readNext = 0;

void setup() {
  Serial.begin(57600);
  initAdc(true);
  initMotors();
  //setupImuTimer();
  ssdInit();
  while (!Serial);
  Serial.println("ok");  
}

void loop() {
  int dst0 = interpretDistanceSensor(_avg0.readAvg(), 49);
  int dst1 = interpretDistanceSensor(_avg1.readAvg(), 49);
  ssdDisplayInt(dst0);
  if(dst0 < 10 || dst1 < 10){
    brakeReward();
    turnRight();
    brake();
    transition(500, 10);//go forward again
  }
}

void setupImuTimer(){
  //disable interrupts for config
  cli();
  //normal waveform generation mode, no output enabled
  TCCR3A = 0;
  //prescaler 256
  TCCR3B = (1 << CS32);
  //enable overflow interrupt
  TIMSK3 |= (1 << TOIE3);
  //reenable interrupts
  sei();
}

ISR (TIMER3_OVF_vect)
{
  Serial.println("of");
  //_gyro.readAndProcessGyro();
}

ISR(ADC_vect){
  if(_readNext == 0){
    _avg0.write(ADC);
    _readNext = 1;
    setAdmux(_readNext);
  }
  else{
    _avg1.write(ADC);
    _readNext = 0;
    setAdmux(_readNext);
  }
}
