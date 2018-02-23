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
 * testing new naming of files
 */

	uint8_t nack = 0;

	uint8_t testWrite[200] = {};
	uint8_t i = 0;
	for(i = 0; i < 200; i++){
		testWrite[i] = 200 - i;
	}
	nack += ne_intel_write(0, 0, 200, testWrite);

	uint8_t testRead[250] = {};
	nack += ne_intel_read(0, 0, 250, testRead);

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
