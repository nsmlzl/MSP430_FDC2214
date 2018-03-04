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

// timer
void timerStart(uint16_t measurementPeriod);
void timerStop();

volatile uint32_t milliSeconds = 0;


uint16_t main(void){
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	// turn LEDs on
	ledOn();

/*
 * testing fdc
 */
	/*
	uint8_t err = 0;
	uint32_t tmpCapacity = 0;
	err += nc_init();

	while(1){

		tmpCapacity = 0;
		err += nc_get_capacity(&tmpCapacity, 0);

	}
	*/

/*
 * testing timer
 */

	timerStart(1000);
	__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, enable interrupts
	__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, enable interrupts
	__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, enable interrupts
	timerStop();

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


// timer init for x-axis and intervall measurement timer
void timerStart(uint16_t measurementPeriod){
	milliSeconds = 0;
	// timer A0.0 for x-axis
	TA0CCTL0 = CCIE;
	TA0CCR0 = 32;
	// timer A1.0 for measurement intervall
	if(measurementPeriod > 2047){
		// else overflow would happen
		measurementPeriod = 2047;
	}
	TA1CCTL0 = CCIE;
	TA1CCR0 = 32 * measurementPeriod;

	// start timer
	TA0CTL = TASSEL_1 + ID_0 + MC_1 + TACLR;
	TA1CTL = TASSEL_1 + ID_0 + MC_1 + TACLR;
}


// stop timer A0 and A1 (both timers)
void timerStop(){
	TA0CCTL0 &= ~CCIE;
	TA1CCTL0 &= ~CCIE;
}

// timer interrupt for x-axis (counts milliSeconds)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(){
	milliSeconds++;
}


// timer interrupt for intervall between measurements
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(){
	// restart after x milliseconds (according to setup of TA0CCR1)
	__bic_SR_register_on_exit(LPM0_bits);

	// check if old measurement wasn't completed -> error
}
