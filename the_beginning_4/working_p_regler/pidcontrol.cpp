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

#include "pidcontrol.h"

int16_t PidControl::calcPwmTarget(uint16_t currentTicksPerSecond)
{
  //tick error
  int16_t err = targetTicksPerSecond - currentTicksPerSecond;
  //Serial.print("err");
  //Serial.println(err);
  proportionalDo(err);
  //Serial.print("pTerm");
  //Serial.println(pTerm);
  //integrativeDo(err);
  //derivativeDo(err);
  return pwmDo();
}

int16_t PidControl::pwmDo()
{
  //TODO: find the correct calculation here
  pwmOutput = lastPwm + (pTerm + iTerm + dTerm);
  //Serial.print("pt");
  //Serial.println(pTerm);
  pwmOutput = pwmOutput > 1023 ? 1023 : pwmOutput;
  pwmOutput = pwmOutput < 40 ? 40 : pwmOutput;
  lastPwm = pwmOutput;
  return pwmOutput;
}

void PidControl::proportionalDo(int16_t err)
{
  pTerm = PROPORTIONAL_FACTOR * (float) err;
}

void PidControl::derivativeDo(int16_t err)
{
  dTerm = DERIVATIVE_FACTOR * (lastError - (float) err);
  lastError = err;
}

int16_t PidControl::getTarget()
{
  return targetTicksPerSecond;
}

void PidControl::integrativeDo(int16_t err)
{
  integrativeTemp += err;
  integrativeTemp = integrativeTemp > INTEGRAL_MAXIMUM ? INTEGRAL_MAXIMUM : integrativeTemp;
  integrativeTemp = integrativeTemp < INTEGRAL_MINIMUM ? INTEGRAL_MINIMUM : integrativeTemp;
  iTerm = INTEGRAL_FACTOR * integrativeTemp;
}

void PidControl::setTarget(int16_t ticksPerSecond)
{
  targetTicksPerSecond = ticksPerSecond;
}