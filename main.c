/*
 *	Written by Niklas Schmelzle <niklas.schmelzle@gmail.com>
 *	main.c
 *
 *	TODO:
 *
 */

#include <msp430f2274.h>


void delay(){
	for(int i = 0; i < 50; i++){
		__delay_cycles(25000);
	}
}


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	P1DIR = 0xFF;
	while(1){
		P1OUT = 0b1;
		delay();
		P1OUT = 0b10;
		delay();
	}

	return 0;
}
