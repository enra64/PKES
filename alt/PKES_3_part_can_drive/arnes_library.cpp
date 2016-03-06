/*
 * arnes_library.cpp
 * Created: 16-Oct-15 09:34:15
 * Author : Arne
 */

#include "arnes_library.h"
#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

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
