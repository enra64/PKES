#include <Arduino.h>
#include "motor.h"
#include "odometry.h"

Motor mMotorControl;
Odometry mOdometry;
InputHandler mInput;

#define BRAKE_DELAY 1000

void loop() {
  //Serial.print(mOdometry.getCurrentTicks(MOTOR_RIGHT));
  //Serial.print(":");
  //Serial.println(mOdometry.getCurrentTicks(MOTOR_LEFT));
  tabularOutput(true);
  //showCurrentTicks();
  delay(5);
}

void tabularOutput(uint8_t output){
  int16_t lPwmSet = mOdometry.calculatePwmTarget(MOTOR_LEFT, output);
  int16_t rPwmSet = mOdometry.calculatePwmTarget(MOTOR_RIGHT, output);
  mMotorControl.setLeft(lPwmSet);
  mMotorControl.setRight(rPwmSet);
  if(!output)
    return;//istTicks is put out before @ calculatePwmTarget
  Serial.print(lPwmSet);
  Serial.print('\t');
  Serial.print(rPwmSet);
  Serial.print('\n');
}

void batchControl(){
  while(!mInput.isFinished()){
    InputEvent event = mInput.getCurrentEvent();
    switch(event.type){
      case 'd':
	execDriveEvent(event.parameter);
	break;
      case 't':
	execTurnEvent(event.parameter);
	break;
    }
  }
}

void execDriveEvent(int16_t ticks){
  uint32_t currentTicks = mOdometry.getCurrentTicks(MOTOR_RIGHT);
  mMotorControl.setLeft(mOdometry.calculatePwmTarget(MOTOR_LEFT);
  mMotorControl.setRight(mOdometry.calculatePwmTarget(MOTOR_RIGHT);
  if((currentTicks - mInput.getLastStateChangeTicks()) >= ticks){
    mMotorControl.setRight(-150);
    mMotorControl.setLeft(-400);
    delay(30);
    mMotorControl.drive(0);
    _delay_ms(BRAKE_DELAY);
    mInput.nextState();
  }
}

void execTurn(int16_t ticks){
  uint32_t currentTicks = mOdometry.getCurrentTicks(MOTOR_RIGHT);
  mMotorControl.setRight(300);
  if(currentTicks - mInput.getLastStateChangeTicks() >= ticks){
    mInput.nextState();
    mMotorControl.setRight(-150);
    mMotorControl.setLeft(-400);
    _delay_ms(30);
    mMotorControl.drive(0);
    _delay_ms(BRAKE_DELAY);
    mOdometry.reset();
    mInput.nextState();
  }
}

void setup(){
  Serial.begin(57600);
  mMotorControl.init();
  mOdometry.init();
  //mMotorControl.drive(500);
  mInput.getInput();
  mOdometry.setTarget(MOTOR_BOTH, 80);
  Serial.println("init ok");
}

//0/1 is guessed here, simply switch around appropriately
ISR(INT2_vect){
  mOdometry.hit(MOTOR_RIGHT);
}

ISR(PCINT1_vect){
  mOdometry.hit(MOTOR_LEFT);
}
