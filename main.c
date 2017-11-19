/*
 *	main.c
 *
 *	Created: 18.11.2017
 * 	Author : Niklas Schmelzle
 *
 * 	TODO:
 *
 */

#include <msp430f2274.h>
#include "i2c.h"


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	P1DIR = 0xFF;
	while(1){
		test();
	}

	return 0;
}
