/*
 * library.h
 *
 * Created: 16-Oct-15 11:36:28
 *  Author: Arne
 */ 

#ifndef LIBRARY_H_
#define LIBRARY_H_

//processor speed for delay.h
#define F_CPU 16000000L

//IMU - MPU-9150
#define MPU9150_ADDRESS 0x69 // R, value from pkes library
#define MPU9150_SLEEP   0x6B // R/W
#define MPU9150_WHO_AM_I 0x75

#define MPU9150_GYRO_XOUT_H        0x43   // R
#define MPU9150_GYRO_XOUT_L        0x44   // R
#define MPU9150_GYRO_YOUT_H        0x45   // R
#define MPU9150_GYRO_YOUT_L        0x46   // R
#define MPU9150_GYRO_ZOUT_H        0x47   // R
#define MPU9150_GYRO_ZOUT_L        0x48   // R

//includes
#include <stdbool.h>//bool  
#include <avr/io.h>//uint8_t
#include <avr/interrupt.h>//enable interrupts, currently mainly for serial communication

//struct defines
struct imu_data
{
	int16_t x, y, z;
};

//led pins
#define LED_1 PJ1
#define LED_2 PA5
#define LED_3 PA4
#define LED_4 PD3

//general function prototypes
void writeDigital(volatile uint8_t* port, int pin, bool val);
int digitCount(int val);
uint8_t getDigit(double val, int pos);

//imu function prototypes
uint8_t imuReadByte(uint8_t addressByte, uint8_t* buffer);
int8_t imuReadInt(int16_t* buffer, uint8_t low, uint8_t high);
uint8_t imuRead(imu_data *data);
void imuChangeRegister(uint8_t registerAdress, uint8_t data);
void imuEnable(void);
#endif /* LIBRARY_H_ */