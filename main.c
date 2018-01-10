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
	uint8_t testTransmit[5] = {0x1, 0x1, 0x1, 0x2, 0x3};
	uint8_t testReceive[3] = {0x0, 0x0, 0x0};

	nack += nsi_transmit(0x50, 5, testTransmit);
	// wait for write time of EEPROM
	for(int16_t i = 0; i < 160; i++){
		__delay_cycles(1000);
	}
	nack += nsi_transmit_receive(0x50, 2, testTransmit, 3, testReceive);

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
