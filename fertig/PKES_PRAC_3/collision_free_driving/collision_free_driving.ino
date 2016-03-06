#include "GlidingAverage.h"
#include "Motor.h"
#include "ADC.h"
#include "General.h"
#include "SSD.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define SPEED 180

GlidingAverage _avg0, _avg1;
uint8_t _readNext = 0;

void setup() {
  Serial.begin(57600);
  initAdc(true);
  initMotors();
  ssdInit();
  while (!Serial);
  transition(SPEED, 10);
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
    transition(SPEED, 10);//go forward again
  }
}

void turnLeft(){
  transition(0, MOTOR_RIGHT, 3);//turn left
  _delay_ms(700);//wait for turn
}

void turnRight(){
  transition(0, MOTOR_LEFT, 3);//turn left
  _delay_ms(700);//wait for turn
}

void brakeReward(){
    brake();
    transition(-SPEED, 5);//reward
    _delay_ms(500);//wait  
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
