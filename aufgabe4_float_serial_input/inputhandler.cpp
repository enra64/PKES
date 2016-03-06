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

void InputHandler::getInput()
{
  mCommandCount = readInt("how many commands?");
  *mCommands = new InputEvent[mCommandCount];
  Serial.println("please enter commands");
  Serial.println("d->drive, t->turn");
  Serial.println("usage: command char *ENTER* <integer parameter>");
  for(int i = mCommandCount; i>=0; i--){
    *mCommands[i] = readInputEvent();
  }
  Serial.println("input parse complete");
}

InputEvent InputHandler::readInputEvent()
{
  InputEvent newInputEvent;
  while(!Serial.available());
  newInputEvent.type = Serial.read();
  while(!Serial.available());
  switch(newInputEvent.type){
    case 'd':
      newInputEvent.parameter = getTicksForDrive(Serial.parseInt());
      break;
    case 't':
      newInputEvent.parameter = getTicksForTurn(Serial.parseInt());
      break;
  }
  return InputEvent;
}

int16_t InputHandler::getTicksForDrive(int16_t cm){ return ((float) cm) * 2; }
int16_t InputHandler::getTicksForTurn(int16_t degree){ return (float) degree * 0.58; }

uint8_t InputHandler::getCurrentEvent(){ return mCurrentState; }

void InputHandler::nextState(){ 
  mCurrentState++; 
  mLastStateChangeTicks = millis();
}

uint8_t InputHandler::isFinished()
{
  return mCurrentState >= mCommandCount;
}

uint32_t InputHandler::getLastStateChangeTicks(){ return mLastStateChangeTicks; }

int InputHandler::readInt(char* request)
{
  Serial.println(request);
  while(!Serial.available());
  return Serial.parseInt();
}