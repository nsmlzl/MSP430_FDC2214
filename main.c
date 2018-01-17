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

	uint8_t zero[128] = {};
	for(uint16_t a = 0; a < 0x100; a++){
		nack += nse_write((uint8_t) a, 0, 128, zero);
		nack += nse_write((uint8_t) a, 0x80, 128, zero);
	}
	nack += nse_read(0x0, 0x5, 1, &read);


	read = 0;
	nack += nse_read(0xFF, 0xFF, 1, &read);
	nack += nse_single_write(0xFF, 0xFF, 0xFA);
	nack += nse_read(0xFF, 0xFF, 1, &read);
	nack += nse_single_write(0x05, 0x10, 0xA);
	for(uint16_t a = 0; a < 256; a++){
		uint8_t read2[128] = {};
		uint8_t read3[128] = {};
		nack += nse_read((uint8_t) a, 0x0, 128, read2);
		nack += nse_read((uint8_t) a, 0x80, 128, read3);
		for(uint8_t b = 0; b < 128; b++){
			if((read2[b] != 0) || (read3[b] != 0)){
				redLed();
			}
		}
	}

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
