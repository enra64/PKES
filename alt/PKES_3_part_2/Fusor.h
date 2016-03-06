#ifndef INTEGRATOR
#define INTEGRATOR

#include <avr/io.h>
#include "GlidingAverage.h"
#include "Imu.h"
#include "General.h"

#define GYRO_WEIGHT 10
#define TIMER_MS_PER_BUFFER_REFRESH (IMU_TIMER_MS_DELTA_PER_TICK * GL_AVG_SIZE)

//high precision values
#define ANGLE_DIVIDER_SENSITIVITY_0 131
#define ANGLE_DIVIDER_SENSITIVITY_1 66
#define ANGLE_DIVIDER_SENSITIVITY_2 33
#define ANGLE_DIVIDER_SENSITIVITY_3 16

class Accelerometer{
  public:
  Vector Racc;
  void read(){
    MPU_9150_readSensor(&Racc, ACCEL);
  }
};

class Gyroscope{
  public:
  Vector _Rgyro;
  void next(Vector* previousEstimate){
    //get gyro values
    Vector gyroRate;
    MPU_9150_readSensor(&gyroRate, GYRO);    
    float Axz = atan2(previousEstimate->x, previousEstimate->z);
    Axz += IMU_TIMER_MS_DELTA_PER_TICK * gyroRate.x;

    float Ayz = atan2(previousEstimate->y, previousEstimate->z);
    Ayz += IMU_TIMER_MS_DELTA_PER_TICK * gyroRate.y;

    _Rgyro.x = sin(Axz) / sqrt(1 + pow(cos(Axz), 2) * pow(tan(Ayz), 2));
    _Rgyro.y = sin(Ayz) / sqrt(1 + pow(cos(Ayz), 2) * pow(tan(Axz), 2));
    //maybe do rgyro = prevEst if prev z is close to 0
    _Rgyro.z = sign(previousEstimate->z) * sqrt(1 - pow(_Rgyro.x, 2) - pow(_Rgyro.y, 2));
  }
};

class Fusor{
  Accelerometer acc;
  Gyroscope gyro;
  Vector _previousEstimate, _currentEstimate;

  void start(){
    acc.read();
    _previousEstimate = acc.Racc;
  }

  void next(){
    //move current to previous
    _previousEstimate.copy(&_currentEstimate);
    //read new acc, gyro
    acc.read();
    gyro.next(&_previousEstimate);
    _currentEstimate.copy(&acc.Racc);
    gyro._Rgyro.mult(GYRO_WEIGHT);
    _currentEstimate.add(&gyro._Rgyro);
    _currentEstimate.div(1 + GYRO_WEIGHT);
  }
};
#endif
