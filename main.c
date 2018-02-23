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
	char *title = "number1, number2, number3\n";
	uint16_t nr[50][3];

	uint16_t y = 0;
	for(y = 0; y < 50; y++){
		uint8_t x = 0;
		for(x = 0; x < 3; x++){
			nr[y][x] = x;
		}
	}
	/*
	int16_t *testPtr = &nr[0][0];
	uint16_t z = 0;
	for(z = 0; z < 150; z++){
		int16_t printInt = *testPtr;
		__delay_cycles(1);
		testPtr++;
	}
	*/
	nf_createCSV(title, &nr[0][0], 50, 3);

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
