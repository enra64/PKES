/*
 * Copyright 2015 <copyright holder> <email>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */

#include "odometry.h"


void Odometry::init()
{
  //set both to internal pullup
  PORTD |= (1 << PD2);
  PORTJ |= (1 << PJ0);

  //-> any edge on int2 generates interrupt
  EICRA |= (1 << ISC20);
  EIMSK |= (1 << INT2);
  
  //toggle on PCINT15:8 will trigger this interrupt
  PCICR |= (1 << PCIE1);
  PCMSK1 |= (1 << PCINT9);
  sei();
  mOdoLastTime_l_ = millis();
  mOdoLastTime_r_ = millis();
}

uint32_t Odometry::getCurrentTicks(uint8_t motor)
{
  if(motor == MOTOR_RIGHT)
    return mOdoCountCurrent_r_;
  else
    return mOdoCountCurrent_l_;
}

int16_t Odometry::calculatePwmTarget(uint8_t motor)
{
  if(motor == MOTOR_RIGHT)
    return mPid_r_.calcPwmTarget(ticksPerSecond(MOTOR_RIGHT));
  else
    return mPid_l_.calcPwmTarget(ticksPerSecond(MOTOR_LEFT));
}

int16_t Odometry::getTarget(uint8_t motor)
{
  if(motor == MOTOR_RIGHT)
    return mPid_r_.getTarget();
   else
    return mPid_l_.getTarget();
}

uint16_t Odometry::ticksPerSecond(uint8_t motor)
{
  uint16_t deltaTime = millis() - (motor == MOTOR_RIGHT ? mOdoLastTime_r_ : mOdoLastTime_l_);
  //Serial.print("ms");
  //Serial.println(millis());
  //Serial.print("dT:");
  //Serial.println(deltaTime);
  uint16_t deltaTicks = (motor == MOTOR_RIGHT ? mOdoCountCurrent_r_ : mOdoCountCurrent_l_) - (motor == MOTOR_RIGHT ? mOdoCountLast_r_ : mOdoCountLast_l_);
  //Serial.print("dTk:");
  //Serial.println(deltaTicks);
  if(motor == MOTOR_RIGHT){
    mOdoCountLast_r_ = mOdoCountCurrent_r_;
    mOdoLastTime_r_ = millis();
  }
  else{
    mOdoCountLast_l_ = mOdoCountCurrent_l_;
    mOdoLastTime_l_ = millis();
  }
  //Serial.print("res");
  //Serial.print(lIstTicks);
  //belongs to tabular output
  //Serial.print((1000 * deltaTicks) / deltaTime);
  //Serial.print('\t');
  return (1000 * deltaTicks) / deltaTime;//equivalent to (deltaTime / 1000) -> seconds
}

void Odometry::setTarget(uint8_t motor, int16_t targetTicksPerSecond)
{
  if(motor == MOTOR_RIGHT)
    mPid_r_.setTarget(targetTicksPerSecond);
  else if (motor == MOTOR_LEFT)
    mPid_l_.setTarget(targetTicksPerSecond);
  else if (motor == MOTOR_BOTH) {
    mPid_r_.setTarget(targetTicksPerSecond);
    mPid_l_.setTarget(targetTicksPerSecond);
  }
}

void Odometry::hit(uint8_t motor)
{
  if(motor == MOTOR_RIGHT)
    mOdoCountCurrent_r_++;
  else
    mOdoCountCurrent_l_++;
}