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
#include <string.h>
#include <stdio.h>
#include "include/ni2c.h"
#include "include/neeprom.h"
#include "include/nmultiplexer.h"
#include "include/nfdc.h"

#include "nfile/nfile.h"

void error();
void red_led();
void green_led();
void led_on();
void led_off();

// timer
void timer_start(uint16_t measurementPeriod);
void timer_stop();

// button interrupt P2.1
void btn_interrupt_init();

// measurement functions
void single_channel_measurement(uint8_t sChannel, uint16_t nrData, uint32_t intervall);

volatile uint32_t milliSeconds = 0;
volatile uint8_t stpMeasurement = 0;
volatile uint8_t err = 0;

uint8_t currentlyMeasuring = 0;



uint16_t main(void){
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	led_on();
	btn_interrupt_init();

	single_channel_measurement(3, 250, 80);

	// led_off();

	return 0;
}


void single_channel_measurement(uint8_t sChannel, uint16_t nrData, uint32_t intervall){
	// set multiplexer for sChannel
	err = nm_set(0, sChannel);
	err = nm_set(1, sChannel);

	uint32_t tmpFreq = 0;

	// init fdc
	err = nc_init();

	// init .csv file
	char *title = "time [ms], frquency data\n";
	nf_init(title);

	// start both timer interrupts
	timer_start(intervall);
	uint8_t counter = 0;
	for(counter = 0; counter < nrData; counter++){
		// so interrupt knows intervall is too fast
		currentlyMeasuring = 1;

		// stop measurement after button press
		if(stpMeasurement){
			break;
		}

		tmpFreq = 0;
		err = nc_get_freq(&tmpFreq, sChannel);

		// create .csv line
		char tmpLine[24] = {};
		sprintf(tmpLine, "%lu, %lu\n", milliSeconds, tmpFreq);
		nf_add_line(tmpLine);


		currentlyMeasuring = 0;
		__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, enable interrupts
	}

	timer_stop();
	if(err){
		red_led();
	}
	else{
		led_off();
	}

	nf_publish();
}


// Led function
// Error function turns red LED on
void error(){
	red_led();
}


// Function turns red LED on
void red_led(){
	P1DIR |= 0x1;
	P1OUT |= 0x1;
	P4OUT &= ~(0x1 << 7);
}


// Function turns green LED on
void green_led(){
	P4DIR |= 0x80;
	P4OUT |= 0x80;
	P1OUT &= ~0x1;
}


// Function turns both LED on
void led_on(){
	P1DIR |= 0x1;
	P1OUT |= 0x1;
	P4DIR |= 0x80;
	P4OUT |= 0x80;
}


// Function turns both LED off
void led_off(){
	P1OUT &= ~0x1;
	P4OUT &= ~(0x1 << 7);
}


// timer init for x-axis and intervall measurement timer
void timer_start(uint16_t measurementPeriod){
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
void timer_stop(){
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
	// intervall would be too fast for measurement, if this happens
	if(currentlyMeasuring){
		err = 20;
	}
}


// init button interrupt p2.1
void btn_interrupt_init(){
	// set P2.1 to input
	P2DIR &= ~BIT1;
	// enable P2.1 internal resistance
	P2REN |= BIT1;
	// set P2.1 as pull-Up resistance
	P2OUT |= BIT1;
	// set etch select for P2.1 (high to low)
	P2IES |= BIT1;
	// reset old interrupt for P2.1
	P2IFG &= ~BIT1;
	// set interrupt for P2.1
	P2IE |= BIT1;
}


// interrupt function for port2
// (P2.1)
#pragma vector = PORT2_VECTOR
__interrupt void PORT_2(){
	stpMeasurement = 1;
	P2IE &= ~BIT1;														// only one single interrupt
}
