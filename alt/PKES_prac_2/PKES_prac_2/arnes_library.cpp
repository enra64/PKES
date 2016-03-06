/*
 * arnes_library.cpp
 * Created: 16-Oct-15 09:34:15
 * Author : Arne
 */

#include "arnes_library.h"
#include "arnes_fifo_buffer.h"
#include "arnes_serial.h"
#include "arnes_twi.h"
#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

/************************************************************************/
/* begin IMU section                                                */
/************************************************************************/

void imuChangeRegister(uint8_t registerAddress, uint8_t data){
	//first, write the address
	Twi::twiWriteTo(MPU9150_ADDRESS, MPU9150_SLEEP, false, false);
	//write the new data now
	Twi::twiWriteTo(MPU9150_ADDRESS, data, true, true);
}

void imuEnable(void){
	imuChangeRegister(MPU9150_SLEEP, 0);
}

/*
	desc		read a byte from the imu (i2c address is hardcoded)
	input		addressByte: the addresson the i2c we want to read
				buffer: buffer to write the result into
	output		added errorcodes from the address write and result read
*/
uint8_t imuReadByte(uint8_t addressByte, uint8_t* buffer){
	//begin with sending a start, test for unrepeated success, no stop condition
	uint8_t writeError = Twi::twiWriteTo(MPU9150_ADDRESS, addressByte, false, false);
	//read
	uint8_t readError = Twi::twiReadFrom(MPU9150_ADDRESS, buffer, true, true);
	return writeError + readError;
}

/* errorlist for something
 * 0: success
 * 1: TWI start condition error
 * 2: TWI address transmission error 
 * 3: TWI first repeated start error
 * 4: IMU low byte send error
 */

/*
	desc		read an int by reading high and low byte from the imu
	input		buffer: space for result
				low: low address byte on imu
				high: high address byte on imu
	output		added errors from both byte reads
*/
int8_t imuReadInt(int16_t* buffer, uint8_t low, uint8_t high){
	uint8_t err = 0;
	//LOW BYTE section
	uint8_t lowResult;
	err += imuReadByte(low, &lowResult);
	
	//get high byte
	uint8_t highResult;
	err += imuReadByte(high, &highResult);
	
	*buffer = (int16_t)((highResult << 8) + lowResult);
	return err;
}

uint8_t imuRead(imu_data *data){
	uint8_t err = 0;
	err += imuReadInt(&data->x, MPU9150_GYRO_XOUT_L, MPU9150_GYRO_XOUT_H);
	err += imuReadInt(&data->y, MPU9150_GYRO_YOUT_L, MPU9150_GYRO_YOUT_H);
	err += imuReadInt(&data->z, MPU9150_GYRO_ZOUT_L, MPU9150_GYRO_ZOUT_H);
	return err;
}

/************************************************************************/
/* end i2c/TWI section                                                */
/************************************************************************/

/*
 * Write high or low to a pin on a port
 * writeDigital(&PORTC, PC2, true)
 */
void writeDigital(volatile uint8_t* port, int pin, bool val){
	if(val)
		*port |=  (1<<pin);//set bit
	else
		*port &=~ (1<<pin);//delete bit
}

//fastest way to do it btw
int digitCount (int n) {
	//lets hope n is != minint, because in c, that fucks up everything
    if (n < 0) n = -n;
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
    /*      2147483647 is 2^31-1 - add more ifs as needed
       and adjust this final return as well. */
    return 10;
}

/*get digit from number; negative values are ignored*/
uint8_t getDigit(double val, int pos){
	// 543210.123456 //after point is negative position
	//be positive!
	if(val < 0)
		val = -val;
	//is before point
	if(pos >= 0)
		return (int)(((int) val) / pow(10,pos)) % 10;
	//is after point
	return (val - (int) val) * pow(10, -pos);
}