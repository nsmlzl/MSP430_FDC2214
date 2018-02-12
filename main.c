/*
 * main.c
 *
 * Created: 2017-11-18
 * Author : Niklas Schmelzle
 *
 * TODO:
 *
 *
 */

#include <msp430f5529.h>
#include <inttypes.h>
#include "include/nsi2c.h"
#include "include/nseeprom.h"
#include "include/nsmultiplexer.h"

#include "nfile/nfile.h"

void error();
void redLed();
void greenLed();
void ledOn();
void ledOff();


uint16_t main(void){
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	// turn LEDs on
	ledOn();

/*
 * testing nfile
 */
	nTestfile();

	/*
	while(1){
	}
	*/

	ledOff();
	return 0;
}


// Led function
// Error function turns red LED on
void error(){
	redLed();
}


// Function turns red LED on
void redLed(){
	P1DIR |= 0x1;
	P1OUT |= 0x1;
	P4OUT &= ~(0x1 << 7);
}


// Function turns green LED on
void greenLed(){
	P4DIR |= 0x80;
	P4OUT |= 0x80;
	P1OUT &= ~0x1;
}


// Function turns both LED on
void ledOn(){
	P1DIR |= 0x1;
	P1OUT |= 0x1;
	P4DIR |= 0x80;
	P4OUT |= 0x80;
}


// Function turns both LED off
void ledOff(){
	P1OUT &= ~0x1;
	P4OUT &= ~(0x1 << 7);
}
