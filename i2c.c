/*
 *	i2c.c
 *
 *	Created: 19.11.2017
 * 	Author : Niklas Schmelzle
 *
 * 	TODO:
 *
 */

#include "i2c.h"

void delay(){
	for(int i = 0; i < 50; i++){
		__delay_cycles(25000);
	}
}


void test(){
	P1OUT = 0b1;
	delay();
	P1OUT = 0b10;
	delay();
}
