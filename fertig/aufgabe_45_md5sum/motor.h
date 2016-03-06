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

#ifndef MOTOR_H
#define MOTOR_H

#define __AVR_ATmega2560__
#define F_CPU 16000000

#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>

#define MOT_1A PB5//IN4 = D11/OC1A, OUT4 MOT_1A
#define MOT_1B PB6//IN3 = D12/OC1B, OUT3 MOT_1B
#define MOT_2A PH3//IN2 = D6/OC4A, OUT2 MOT_2A
#define MOT_2B PH4//IN1 = D7/OC4B, OUT1 MOT_2B

//motor selection, forward/reward
#define FOR_1 0 //rechtes rad, nach links
#define REW_1 1 //rechtes rad, nach rechts
#define FOR_2 2 //linkes rad, nach links
#define REW_2 4 //linkes rad, nach rechts

#define MOTOR_LEFT 2
#define MOTOR_BOTH 0
#define MOTOR_RIGHT 1

class Motor {
private:
    int16_t _speed1 = 0, _speed2 = 0;
    uint8_t _direction1, _direction2;
public:
    void init(void);

    void setMotor(uint16_t value, uint8_t mode);

    void setRight(int16_t value);

    void setLeft(int16_t value);

    void saveSpeed(uint16_t value, uint8_t mode);

    void drive(int16_t value, bool isLeft);

    void drive(int16_t value);

    void reward(uint16_t value);

    void stop();

    void transition(int16_t target, uint8_t which, int8_t delta);

    void transition(int16_t target, uint8_t delta);

    void forceBrake();
};

#endif // MOTOR_H
