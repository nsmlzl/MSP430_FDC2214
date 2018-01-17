/*
 * main.c
 *
 * Created: 18.11.2017
 * Author : Niklas Schmelzle
 *
 * TODO:
 *
 */

#include <msp430f5529.h>
#include <inttypes.h>
#include "include/nsi2c.h"
#include "include/nseeprom.h"

void error();
void redLed();
void greenLed();
void ledOn();
void ledOff();


int16_t main(void){
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	// turn LEDs on
	ledOn();

/*
 * testing of own USCI Library
 */
	int16_t nack = 0;
	uint8_t read = 0;

	uint8_t testRead[500] = {};
	uint8_t testRead2[128] = {};
	uint8_t testRead3[128] = {};
	uint8_t testRead4[128] = {};
	uint8_t testData[256] = {};

	for(uint16_t i = 0; i < 256; i++){
		testData[i] = i;
	}

	nack += nse_intel_write(0x0, 0x0, 256, testRead);
	nack += nse_intel_write(0x0, 0x81, 256, testData);
	nack += nse_intel_read(0x0, 0x0, 500, testRead);

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
