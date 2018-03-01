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
#include <stdlib.h>
#include "include/ni2c.h"
#include "include/neeprom.h"
#include "include/nmultiplexer.h"
#include "include/nfdc.h"

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
 * testing fdc
 */
	uint8_t err = 0;
	uint32_t tmpCapacity = 0;
	err += nc_init();
	err += nc_get_capacity(&tmpCapacity, 3);

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
