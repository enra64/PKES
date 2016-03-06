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

#include "inputhandler.h"

void InputHandler::getInput() {
    mCommandCount = readInt("how many commands?");
    mCommands = new InputEvent[3];
    Serial.print("please enter ");
    Serial.print(mCommandCount);
    Serial.println(" commands");
    Serial.println("d->drive, t->turn");
    Serial.println("usage: command char *ENTER* <integer parameter>");
    for (int i = 0; i < mCommandCount; i++) {
        mCommands[i] = readInputEvent();
        printInputEvent(&mCommands[i]);
        mPrintedEvent = false;//reset, variable is used elsewhere
    }
    Serial.println("input parse complete\n");
}

InputEvent InputHandler::readInputEvent() {
    InputEvent newInputEvent;
    while (!Serial.available());
        newInputEvent.type = Serial.read();
    while (!Serial.available());
    switch (newInputEvent.type) {
        case 'd':
            newInputEvent.parameter = getTicksForDrive((int16_t) Serial.parseInt());
            break;
        case 't':
        default:
            newInputEvent.parameter = getTicksForTurn((int16_t) Serial.parseInt());
    }
    return newInputEvent;
}

int16_t InputHandler::getTicksForDrive(int16_t cm) {
    return (int16_t) (cm * 2.0);
}

int16_t InputHandler::getTicksForTurn(int16_t degree) {
    return (int16_t) (degree * 0.58);
}

InputEvent InputHandler::getCurrentEvent() {
    return mCommands[mCurrentState];
}

void InputHandler::nextState(uint32_t currentTicks) {
    mCurrentState++;
    mPrintedEvent = false;
    mLastStateChangeTicks = currentTicks;
}

bool InputHandler::isFinished() {
    return mCurrentState >= mCommandCount;
}

uint32_t InputHandler::getLastStateChangeTicks() {
    return mLastStateChangeTicks;
}

int16_t InputHandler::readInt(const char *request) {
    Serial.println(request);
    while (!Serial.available());
    return (int16_t) Serial.parseInt();
}

void InputHandler::printInputEvent(InputEvent *event) {
    if(mPrintedEvent)
        return;
    mPrintedEvent = true;
    switch(event->type){
        case 'd':
            if(event->parameter < 0){
                Serial.print("Drive backward for ");
                Serial.print(-event->parameter);
            }
            else{
                Serial.print("Drive forward for ");
                Serial.print(event->parameter);
            }
            Serial.println(" ticks");
            break;
        case 't':
        default:
            Serial.print("Turn ");
            if(event->parameter < 0){
                Serial.print(-event->parameter);
                Serial.println(" ticks left");
            }
            else{
                Serial.print(event->parameter);
                Serial.println(" ticks right");
            }
    }
}

void InputHandler::resetStateChangeTicks() {
    mLastStateChangeTicks = 0;
}
