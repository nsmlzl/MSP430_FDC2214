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

	uint8_t sendTest1[30] = {};
	uint8_t sendTest2[128] = {};
	uint8_t sendTest3[128] = {};
	uint8_t readTest[50] = {};

	for(uint16_t i = 0; i < 30; i++){
		sendTest1[i] = 255;
	}

	for(uint16_t i = 0; i < 128; i++){
		sendTest2[i] = i;
		sendTest3[i] = i + 128;
	}

	nack += nse_write(0x0, 0x0, 30, sendTest1);
	// wait for write time of EEPROM
	for(int16_t i = 0; i < 160; i++){
		__delay_cycles(1000);
	}
	nack += nse_read(0x0, 0x0, 50, readTest);

	nack += nse_write(0x0, 0x14, 128, sendTest2);
	// wait for write time of EEPROM
	for(int16_t i = 0; i < 160; i++){
		__delay_cycles(1000);
	}
	nack += nse_write(0x0, 0x94, 128, sendTest3);
	// wait for write time of EEPROM
	for(int16_t i = 0; i < 160; i++){
		__delay_cycles(1000);
	}
	for(int i = 0; i < 6; i++){
		nack += nse_read(0x0, 0x0 + 50 * i, 50, readTest);
		__delay_cycles(1);
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
