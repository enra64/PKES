#ifndef MOTOR
#define MOTOR

#include <avr/io.h>
#include "Arduino.h"
//motor 1
//IN4 = D11/OC1A, OUT4 MOT_1A
#define MOT_1A PB5
//IN3 = D12/OC1B, OUT3 MOT_1B
#define MOT_1B PB6

//motor 2
//IN2 = D6/OC4A, OUT2 MOT_2A
#define MOT_2A PH3
//IN1 = D7/OC4B, OUT1 MOT_2B
#define MOT_2B PH4

//motor selection, forward/reward
#define FOR_1 0 //rechtes rad, nach links
#define REW_1 1 //rechtes rad, nach rechts
#define FOR_2 2 //linkes rad, nach links
#define REW_2 4 //linkes rad, nach rechts

#define MOTOR_LEFT 2
#define MOTOR_BOTH 0
#define MOTOR_RIGHT 1

int16_t _speed1 = 0, _speed2 = 0;

void initMotors(void){
  //output
  DDRB |= (1 << MOT_1A) | (1 << MOT_1B);
  DDRH |= (1 << MOT_2A) | (1 << MOT_2B);
  
  //clock select in control register b, also set WGMx2 for fast PWM
  //b only contains input capture stuff, WGM3:2 and CS
  TCCR1B |= (1 << CS11) | (1 << WGM12);//| (1 << WGM13);
  TCCR4B |= (1 << CS41) | (1 << WGM42);//| (1 << WGM43);//fast pwm, mode 7. using mode 15 we can change the top value, not the output voltage

  //set remaining bits in A register to enable 10 bit fast pwm
  //A register only has this and compare output mode for A to C, which must be set per motor
  TCCR1A |= (1 << WGM11) | (1 << WGM10);
  TCCR4A |= (1 << WGM41) | (1 << WGM40);
}

void saveSpeed(uint16_t value, uint8_t mode){
  switch(mode){
    case FOR_1:
      _speed1 = value;
      break;
    case REW_1:
      _speed1 = -value;
      break;
    case FOR_2:
      _speed2 = value;
      break;
    case REW_2:
      _speed2 = -value;
      break;
  }
}

void setMotor(uint16_t value, uint8_t mode){
  saveSpeed(value, mode);
  //clamp to 10 bit, invert for higher value -> higher voltage
  value = 1023 - (value & 0x3FF);
  //setting COMx(A/B)1:0 switches PWM on on (A/B), for COMx(A/B)0:1 = 0 PWM is off on that port
  switch(mode){
    case FOR_1:
      //disable reward drive (on b)
      TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));
      //enable forward drive (on a)
      TCCR1A |= (1 << COM1A1) | (1 << COM1A0);
      OCR1A = value;
      break;
    //motor 1, timer 1 B
    case REW_1:
      //enable reward drive (on b)
      TCCR1A |= ((1 << COM1B1) | (1 << COM1B0));
      //disable forward drive (on a)
      TCCR1A &= ~(1 << COM1A1) | (1 << COM1A0);
      OCR1B = value;
      break;
    //motor 2, timer 4 A
    case FOR_2:
      //disable reward drive (on b)
      TCCR4A &= ~((1 << COM4B1) | (1 << COM4B0));
      //enable forward drive (on a)
      TCCR4A |= (1 << COM4A1) | (1 << COM4A0);
      OCR4A = value;
      break;
    //motor 2, timer 4 B
    case REW_2:
      //enable reward drive (on b)
      TCCR4A |= ((1 << COM4B1) | (1 << COM4B0));
      //disable forward drive (on a)
      TCCR4A &= ~(1 << COM4A1) | (1 << COM4A0);
      OCR4B = value;
      break;
  }
}

//swapped, motor 1 is right
void setRight(int16_t value){
  if(value < 0)
    setMotor(-value, REW_2);
  else
    setMotor(value, FOR_2);
}

void setLeft(int16_t value){
  if(value < 0)
    setMotor(-value, REW_1);
  else
    setMotor(value, FOR_1);
}

void drive(int16_t value, bool isLeft){
  if(isLeft)
    setLeft(value);
  else
    setRight(value);
}

void drive(int16_t value){
  setLeft(value);
  setRight(value);
}

void reward(uint16_t value){
  setMotor(value, REW_1);
  setMotor(value, REW_2);
}

void stop(){
  setMotor(0, FOR_1);
  setMotor(0, FOR_2);
}

//blocking!
void transition(int16_t target, uint8_t which, int8_t delta){
  int16_t cur;
  if(which == MOTOR_LEFT)
    cur = _speed1;
  else if(which == MOTOR_RIGHT)
    cur = _speed2;
  else
    cur = (_speed1 + _speed2) / 2;
  //negative delta if we want to reduce speed, eg cur > target
  delta = cur < target ? delta : -delta;
  for(int16_t i = cur; i != target; i+=delta){//subtract or add speed
    //check whether we have reached our target
    if(delta < 0 && i <= target)  break;
    else if (delta > 0 && i >= target)  break;
    /*Serial.print(delta);
    Serial.print('.');
    Serial.print(i);
    Serial.print(':');
    Serial.print(target);
    Serial.print(';');*/
    //set motors
    if(which == MOTOR_BOTH)  drive(i);
    else if (which == MOTOR_LEFT)  drive(i, true);
    else  drive(i, false);
    _delay_ms(1);
  }
}

void transition(int16_t target, uint8_t delta){
  transition(target, MOTOR_BOTH, delta);
}

void brake(){
  transition(0,5);//brake
}

#endif
