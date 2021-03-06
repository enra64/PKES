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

int16_t PidControl::calcPwmTarget(float currentTicksPerSecond) {
    //tick error
    float err = targetTicksPerSecond - currentTicksPerSecond;
    //Serial.print("err");
    if (!isnan(currentTicksPerSecond)) {
        proportionalDo(err);
        integrativeDo(err);
        derivativeDo(err);
    }
    Serial.println(err);
    //Serial.print("iTerm");
    //Serial.println(iTerm);
    return pwmDo();
}

int16_t PidControl::pwmDo() {
    //TODO: find the correct calculation here
    pwmOutput = (int16_t) (lastPwm + (pTerm + iTerm + dTerm));

    pwmOutput = pwmOutput > 1023 ? 1023 : pwmOutput;
    pwmOutput = pwmOutput < 120 ? 120 : pwmOutput;
    lastPwm = pwmOutput;
    return pwmOutput;
}

void PidControl::reset() {
    lastError = 0;
    integrativeTemp = 0;
    lastPwm = 0;
    pwmOutput = 0;
    pTerm = 0;
    iTerm = 0;
    dTerm = 0;
}

void PidControl::proportionalDo(float err) {
    pTerm = PROPORTIONAL_FACTOR * err;
}

void PidControl::derivativeDo(float err) {
    //Serial.println(dTerm);
    dTerm = DERIVATIVE_FACTOR * (lastError - err);
    //Serial.println(err);
    lastError = err;
}

int16_t PidControl::getTarget() {
    return targetTicksPerSecond;
}

void PidControl::integrativeDo(float err) {
    //Serial.print(integrativeTemp);
    //Serial.print('\t');
    integrativeTemp += err * INTEGRAL_FACTOR;
    //Serial.println(err);
    integrativeTemp = integrativeTemp > INTEGRAL_MAXIMUM ? INTEGRAL_MAXIMUM : integrativeTemp;
    integrativeTemp = integrativeTemp < INTEGRAL_MINIMUM ? INTEGRAL_MINIMUM : integrativeTemp;
    iTerm = integrativeTemp;
}

void PidControl::setTarget(int16_t ticksPerSecond) {
    targetTicksPerSecond = ticksPerSecond;
}