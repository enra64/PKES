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


Odometry::init()
{
  EICRA = (1 << ISC20);//-> any edge on int2 generates interrupt
  PCICR = (1 << PCIE1);//toggle on PCINT15:8 will trigger this interrupt
  mOdoLastTime_l_ = millis();
  mOdoLastTime_r_ = millis();
}

int16_t Odometry::getPwmTarget(uint8_t isRight)
{
  if(isRight)
    return mPid_r_.getPwmTarget(ticksPerSecond(true));
  else
    return mPid_l_.getPwmTarget(ticksPerSecond(false));
}

uint16_t Odometry::ticksPerSecond(uint8_t isRight)
{
  uint16_t deltaTime = (isRight ? mOdoLastTime_r_ : mOdoLastTime_l_) - millis();
  uint16_t deltaTicks = (isRight ? mOdoCountCurrent_r_ : mOdoCountCurrent_l_) - (isRight ? mOdoCountLast_r_ : mOdoCountLast_l_);
  if(isRight){
    mOdoCountLast_r_ = mOdoCountCurrent_r_;
    mOdoLastTime_r_ = millis();
  }
  else{
    mOdoCountLast_l_ = mOdoCountCurrent_l_;
    mOdoLastTime_l_ = millis();
  }
  return (1000 * deltaTicks) / deltaTime;//equivalent to (deltaTime / 1000) -> seconds
}

void Odometry::setTarget(uint8_t isRight, int16_t target)
{
  if(isRight)
    mPid_r_ = target;
  else if (isRight == 0)
    mPid_l_ = target;
  else{
    mPid_r_ = target;
    mPid_l_ = target;
  }
}

void Odometry::hit(uint8_t isRight)
{
  if(isRight)
    mOdoCountCurrent_r_++;
  else
    mOdoCountCurrent_l_++;
}