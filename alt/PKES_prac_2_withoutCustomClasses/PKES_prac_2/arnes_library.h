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

//serial registers
#define SERIAL_0_STATUS UCSR0A
#define SERIAL_0_CONFIG_1 UCSR0B
#define SERIAL_0_CONFIG_2 UCSR0C
//serial values
#define BAUD 57600
#define SERIAL_BUFFER_SIZE 64

//IMU - MPU-9150
#define MPU9150_ADDRESS 0x69 // R
#define MPU9150_SLEEP   0x6B // R/W

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
	uint16_t x, y, z;
};

//seven segment display pins
#define SEVEN_SEGMENT_CLOCK PC2
#define SEVEN_SEGMENT_DATA PC3
#define SEVEN_SEGMENT_DATA_ENABLE PC4

//led pins
#define LED_1 PJ1
#define LED_2 PA5
#define LED_3 PA4
#define LED_4 PD3

const uint8_t NUMBERS_7SD[] = {
	0b11111100,
	0b01100000,
	0b11011010,
	0b11110010,
	0b01100110,
	0b10110110,
	0b10111110,
	0b11100000,
	0b11111110,
	0b11110110,
	0b00000010};

//7SD function prototypes
void writeDigital(volatile uint8_t* port, int pin, bool val);
void send7SDBit(bool val);
void send7SDTransmission(unsigned char dataBytes[]);
void displayNumber(double val);
void displayNumber(int val);

//serial function prototypes
bool serialIsDataAvailable_0(void);
bool serialIsDataSent_0(void);
unsigned char serialRead_0(void);
void serialWrite_0(unsigned char dataOut);
void serialWrite_0(const char dataOut[], uint8_t size);
void serialWriteString_0(const char dataOut[]);
void serialInit_0(void);
int digitCount(int val);
uint8_t getDigit(double val, int pos);
void serialWriteInt16_0(int val);
//serial interrupts
void serialEnableInterrupts_0(bool rx, bool tx);
void serialWriteUint16_0(uint16_t val);

//twi function prototypes
void twiInitialize(void);
bool twiStart(void);
bool twiStart(bool repeatedStart);
void twiStop(void);
void twiWriteByte(uint8_t val);
void twiReadAck(uint8_t *data);
uint8_t twiWriteTo(uint8_t address, uint8_t data, bool repeated, bool stop);
uint8_t twiReadFrom(uint8_t address, uint8_t* data, bool repeated, bool stop);
int16_t imuReadInt(uint8_t low, uint8_t high);
void imuRead(imu_data *data);
void imuChangeRegister(uint8_t registerAdress, uint8_t data);
void imuEnable(void);

#endif /* LIBRARY_H_ */