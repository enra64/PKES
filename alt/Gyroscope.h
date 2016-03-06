#ifndef GYRO_H_
#define GYRO_H_

#include <avr/io.h>
#include "GlidingAverage.h"
#include "Imu.h"
#include "General.h"

#define GYRO_AVERAGE_SIZE 16
#define TIMER_DELTA_PER_TICK 1
#define TIME_DELTA_ANGLE_MEASUREMENT TIMER_DELTA_PER_TICK * GYRO_AVERAGE_SIZE
#define X 0
#define Y 1
#define Z 2

//generously rounded
#define ANGLE_DIVIDER_SENSITIVITY_0 131
#define ANGLE_DIVIDER_SENSITIVITY_1 66
#define ANGLE_DIVIDER_SENSITIVITY_2 33
#define ANGLE_DIVIDER_SENSITIVITY_3 16
#define ANGLE_DIVIDER_SENSITIVITY_UNKNOWN 1

class Gyro{
  GlidingAverage _buffer_x, _buffer_y, _buffer_z;
  imu_data _reference, _angle;
  //if writecounter == GYRO_AVERAGE_SIZE, GAS timer ticks are over, so we can calculate another delta angle
  uint8_t _writeCounter = 0;

  int16_t getBufferAverage(uint8_t id){
    switch(id){
      case X:  return _buffer_x.readAvg();
      case Y:  return _buffer_y.readAvg();
      case Z:  return _buffer_z.readAvg();
    }
  }

  int16_t addToAngle(uint8_t id, int16_t value){
    switch(id){
      case X:  
        _angle.x += value;
        return _angle.x;
      case Y:  
        _angle.y += value;
        return _angle.y;
      case Z:  
        _angle.z += value;
        return _angle.z;
    }
  }

  //calculate the delta angle and add it to the current angle
  int16_t calculateAngle(uint8_t id){
    _writeCounter = 0;
    int16_t avgMeasurement = getBufferAverage(id);
    return addToAngle(id, avgMeasurement * GYRO_AVERAGE_SIZE);
  }

  //inject x,y,z gyro data into the appr. averaging buffers, and trigger angle processing all GYRO_AVERAGE_SIZE times
  void injectGyroData(imu_data* data){
    //save the values
    _buffer_x.write(data->x);
    _buffer_y.write(data->y);
    _buffer_z.write(data->z);
    _writeCounter++;

    if(_writeCounter == GYRO_AVERAGE_SIZE)
      calculateAngle(X);
  }
  
  public:
  //init gliding average with size
  Gyro(){
    _reference = {.x = 0, .y = 0, .z = 0};
    _angle = {.x = 0, .y = 0, .z = 0};
  }

  //use this to get current angle
  int16_t getAngle(uint8_t id, uint8_t sensitivity){
    return addToAngle(id, 0) / sensitivity;//adds 0 to current angle and returns the angle...
  }

  //call this from outside to trigger a new gyro measurement
  void readAndProcessGyro(){
    imu_data gyro;
    MPU_9150_readSensor(&gyro, GYRO);
    injectGyroData(&gyro);
  }

  //save the current state of the averaging buffers to be able to subtract this from
  //current values to improve accuracy?
  void createReference(){
    _angle.x = 0;
    _angle.y = 0;
    _angle.z = 0;
    _reference.x = _buffer_x.readAvg();
    _reference.y = _buffer_y.readAvg();
    _reference.z = _buffer_z.readAvg();
  }
};

#endif
