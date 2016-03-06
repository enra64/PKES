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

#define PROPORTIONAL_FACTOR 0.1
#define INTEGRAL_FACTOR 0.1
#define DERIVATIVE_FACTOR 0.1

#define INTEGRAL_MAXIMUM 100
#define INTEGRAL_MINIMUM -100

#include <avr/io.h>

class PidControl
{
private:
  float lastError = 0, integrativeTemp = 0;
  int16_t lastPwm = 0, targetTicksPerSecond = 0, pwmOutput = 0;
  float error = 0, pTerm = 0, iTerm = 0, dTerm = 0;
  float latestTicksPerSeconds = 0;
  void integrativeDo(uint16_t err);
  void derivativeDo(uint16_t err);
  void proportionalDo(uint16_t err);
  int16_t pwmDo();
public:
  void setTarget(int16_t targetTicksPerSecond);
  int16_t getPwmTarget(uint16_t ticksPerSecond);
};

#endif // PIDCONTROL_H
