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

#ifndef ODOMETRY_H
#define ODOMETRY_H

#include <avr/interrupt.h>
#include <Arduino.h>
#include "pidcontrol.h"
#include "motor.h"

//#define _l_ 2
//#define _r_ 1

class Odometry
{
private:
  uint32_t mOdoCountCurrent_r_, mOdoCountCurrent_l_, mOdoCountLast_r_, mOdoCountLast_l_, mOdoLastTime_r_, mOdoLastTime_l_;
  PidControl mPid_l_, mPid_r_;
public:
  uint32_t getCurrentTicks(uint8_t isRight);
  void init();
  void hit(uint8_t isRight);
  float ticksPerMilliSecond(uint8_t motor);
  int16_t calculatePwmTarget(uint8_t isRight);
  int16_t getTarget(uint8_t motor);
  void setTarget(uint8_t isRight, float target);
  void reset();
};

#endif // ODOMETRY_H