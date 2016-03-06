/*
 * Copyright 2016 <copyright holder> <email>
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

#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

//define this shit for arduino and delay and io.h
#define __AVR_ATmega2560__
#define F_CPU 16000000

#include <Arduino.h>

struct InputEvent {
    int32_t parameter = 0;
    int16_t type = 0xFF;
};

class InputHandler {
private:
    InputEvent* mCommands;
    int mCommandCount;
    uint8_t mCurrentState = 0;
    uint32_t mLastStateChangeTicks = 0;
    bool mPrintedEvent = false;

    int16_t getTicksForDrive(int16_t cm);

    int16_t getTicksForTurn(int16_t degree);

    int16_t readInt(const char *request);

    InputEvent readInputEvent();

public:
    void printInputEvent(InputEvent *event);

    uint32_t getLastStateChangeTicks();

    InputEvent getCurrentEvent();

    void nextState(uint32_t currentTicks);

    bool isFinished();

    void resetStateChangeTicks();

    void getInput();
};

#endif // INPUTHANDLER_H
