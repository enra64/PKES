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

#ifndef PIDCONTROL_H
#define PIDCONTROL_H

#define PROPORTIONAL_FACTOR 1 //beware: if the complete error gets below 1.0d, integer addition will be too small!
#define INTEGRAL_FACTOR 0.1
#define DERIVATIVE_FACTOR 0.1

#define INTEGRAL_MAXIMUM 100
#define INTEGRAL_MINIMUM -100

#include <Arduino.h>
#include <avr/io.h>

class PidControl
{
private:
  float lastError = 0, integrativeTemp = 0;
  float lastPwm = 0, pwmOutput = 0;
  float error = 0, pTerm = 0, iTerm = 0, dTerm = 0, targetTicksPerSecond = 0;
  void integrativeDo(float err);
  void derivativeDo(float err);
  void proportionalDo(float err);
  int16_t pwmDo();
public:
  void reset();
  float getTarget();
  void setTarget(float targetTicksPerSecond);
  int16_t calcPwmTarget(float ticksPerSecond);
};

#endif // PIDCONTROL_H
