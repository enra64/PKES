#define __AVR_ATmega2560__
#include <Arduino.h>
#include "motor.h"
#include "odometry.h"
#include "inputhandler.h"

Motor mMotorControl;
Odometry mOdometry;
InputHandler mInput;

#define BRAKE_DELAY 1000

#define DRIVE true
#define TURN false

#define TICKS_KRUSTY 120
#define TICKS_MARGE 100

#define DRIVE_TARGET_TICKS TICKS_KRUSTY

#define TURN_SPEED_MARGE 200
#define TURN_SPEED_KRUSTY 300

#define TURN_SPEED TURN_SPEED_KRUSTY

#define WHAT_ODOMETRY_WORKS MOTOR_LEFT

void tabularOutput(uint8_t output) {
    int16_t lPwmSet = mOdometry.calculatePwmTarget(MOTOR_LEFT, output);
    int16_t rPwmSet = mOdometry.calculatePwmTarget(MOTOR_RIGHT, output);
    mMotorControl.setLeft(lPwmSet);
    mMotorControl.setRight(rPwmSet);
    if (!output)
        return;//istTicks is put out before @ calculatePwmTarget
    Serial.print(lPwmSet);
    Serial.print('\t');
    Serial.print(rPwmSet);
    Serial.print('\n');
}

/**
 * Check whether the targetTicks have been reached, and if so, brake and go to the next state.
 */
void checkForAbort(int16_t targetTicks, uint32_t currentTicks, bool event){
    if ((currentTicks - mInput.getLastStateChangeTicks()) >= targetTicks) {
        mMotorControl.forceBrake();
        _delay_ms(BRAKE_DELAY);
        if(event == DRIVE)
            Serial.println("finished drive event");
        else
            Serial.println("finished turn event");
        Serial.print("current: ");
        Serial.print(currentTicks);
        Serial.print(", last change:");
        Serial.print(mInput.getLastStateChangeTicks());
        Serial.print(", target: ");
        Serial.print(targetTicks);
        Serial.print("\n\n");
        mOdometry.reset();
        mInput.nextState(0);
    }
}

void execDriveEvent(int16_t targetTicks) {
    mOdometry.setTarget(MOTOR_BOTH, DRIVE_TARGET_TICKS);//marge
    uint32_t currentTicks = mOdometry.getCurrentTicks(WHAT_ODOMETRY_WORKS);

    int16_t leftPwm = mOdometry.calculatePwmTarget(MOTOR_LEFT, 0);
    int16_t rightPwm = mOdometry.calculatePwmTarget(MOTOR_RIGHT, 0);

    mMotorControl.setLeft(targetTicks > 0 ? leftPwm : -leftPwm);
    mMotorControl.setRight(targetTicks > 0 ? rightPwm : -rightPwm);

    checkForAbort(targetTicks, currentTicks, DRIVE);
}

void execTurnEvent(int16_t ticks) {
    uint32_t currentTicks = mOdometry.getCurrentTicks(WHAT_ODOMETRY_WORKS);

    if(WHAT_ODOMETRY_WORKS == MOTOR_RIGHT)
        mMotorControl.setRight(ticks > 0 ? 300 : -300);
    else
        mMotorControl.setLeft(ticks > 0 ? TURN_SPEED : -TURN_SPEED);
    checkForAbort(ticks, currentTicks, TURN);
}

void batchControl() {
    while (!mInput.isFinished()) {
        InputEvent event = mInput.getCurrentEvent();
        mInput.printInputEvent(&event);
        switch (event.type) {
            case 'd':
                execDriveEvent(event.parameter);
                break;
            case 't':
            default:
                execTurnEvent(event.parameter);
        }
    }
}

void setup() {
    Serial.begin(57600);
    mMotorControl.init();
    mOdometry.init();
    _delay_ms(20);
    mInput.getInput();
    //mOdometry.setTarget(MOTOR_BOTH, 80);//krusty
    mOdometry.setTarget(MOTOR_BOTH, DRIVE_TARGET_TICKS);//marge
    //mInput.resetStateChangeTicks();

    //mOdometry.setTarget(MOTOR_BOTH, 80);
}

#define BATCH 1

void loop() {
    if(BATCH)
        batchControl();
    else
        tabularOutput(true);

    delay(5);
}

//0/1 is guessed here, simply switch around appropriately
ISR(INT2_vect){
        mOdometry.hit(MOTOR_RIGHT);
}

ISR(PCINT1_vect){
        mOdometry.hit(MOTOR_LEFT);
}
