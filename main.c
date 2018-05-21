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
void measure_function_1();
void measure_function_2();
void measure_function_3();
void measure_function_4();
void measure_function_5();
void measure_function_6();

void single_channel_measurement(uint8_t sChannel, uint16_t nrData, uint32_t intervall);
void single_channel_measurement_without_multiplexer(uint8_t sChannel, uint16_t nrData, uint32_t intervall);

volatile uint32_t milliSeconds = 0;
volatile uint8_t stpMeasurement = 0;
volatile uint8_t errGlob = 0;

uint8_t currentlyMeasuring = 0;



uint16_t main(void){
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	led_on();
	btn_interrupt_init();

	/* Dieser Bereich wird für die Versuche angepasst */
	measure_function_4();

	// led_off();

	return 0;
}


void measure_function_1(){
	uint8_t mchannel = 0;
	single_channel_measurement(mchannel, 600, 50);
}


void measure_function_2(){
	uint8_t mchannel = 0;
	uint8_t function2 = 0;
	if(function2){
		single_channel_measurement(mchannel, 120, 250);
	}
	else{
		single_channel_measurement(mchannel, 600, 50);
	}
}


void measure_function_3(){
	uint8_t mchannel = 0;
	uint8_t function2 = 1;
	if(function2){
		single_channel_measurement_without_multiplexer(mchannel, 600, 50);
	}
	else{
		single_channel_measurement(mchannel, 600, 50);
	}
}


void measure_function_4(){
	measure_function_1();
}


void measure_function_5(){
	measure_function_1();
}


void single_channel_measurement_without_multiplexer(uint8_t sChannel, uint16_t nrData, uint32_t intervall){
	uint32_t tmpFreq = 0;

	// init fdc
	errGlob = nc_init();

	// create dataPtr
	uint32_t *allData = (uint32_t*)malloc(nrData * 2 * sizeof(uint32_t));
	uint32_t *allDataPtr = allData;

	// start both timer interrupts
	timer_start(intervall);
	uint16_t counter = 0;
	for(counter = 0; counter < nrData; counter++){
		// so interrupt knows intervall is too fast
		currentlyMeasuring = 1;

		// stop measurement after button press
		if(stpMeasurement){
			break;
		}

		tmpFreq = 0;
		errGlob = nc_get_freq(&tmpFreq, sChannel);

		*allDataPtr = milliSeconds;
		allDataPtr++;
		*allDataPtr = tmpFreq;
		allDataPtr++;

		currentlyMeasuring = 0;
		__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, enable interrupts
	}

	timer_stop();
	if(errGlob){
		red_led();
	}
	else{
		green_led();
	}

	// create csv file
	char *title = "time [ms], frequency data\n";
	nf_init(title);

	allDataPtr = allData;
	for(counter = 0; counter < nrData; counter++){
		uint32_t allDataTime = *allDataPtr;
		allDataPtr++;
		uint32_t allDataFreq = *allDataPtr;
		allDataPtr++;

		// create .csv line
		char tmpLine[24] = {};
		sprintf(tmpLine, "%lu, %lu\n", allDataTime, allDataFreq);
		nf_add_line(tmpLine);
	}
	free(allData);


	led_off();
	nf_publish();
}


void single_channel_measurement(uint8_t sChannel, uint16_t nrData, uint32_t intervall){
	// set multiplexer for sChannel
	errGlob = nm_set(0, sChannel);
	errGlob = nm_set(1, sChannel);

	uint32_t tmpFreq = 0;

	// init fdc
	errGlob = nc_init();

	// create dataPtr
	uint32_t *allData = (uint32_t*)malloc(nrData * 2 * sizeof(uint32_t));
	uint32_t *allDataPtr = allData;

	// start both timer interrupts
	timer_start(intervall);
	uint16_t counter = 0;
	for(counter = 0; counter < nrData; counter++){
		// so interrupt knows intervall is too fast
		currentlyMeasuring = 1;

		// stop measurement after button press
		if(stpMeasurement){
			break;
		}

		tmpFreq = 0;
		errGlob = nc_get_freq(&tmpFreq, sChannel);

		*allDataPtr = milliSeconds;
		allDataPtr++;
		*allDataPtr = tmpFreq;
		allDataPtr++;

		currentlyMeasuring = 0;
		__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, enable interrupts
	}

	timer_stop();
	if(errGlob){
		red_led();
	}
	else{
		green_led();
	}

	// create csv file
	char *title = "time [ms], frequency data\n";
	nf_init(title);

	allDataPtr = allData;
	for(counter = 0; counter < nrData; counter++){
		uint32_t allDataTime = *allDataPtr;
		allDataPtr++;
		uint32_t allDataFreq = *allDataPtr;
		allDataPtr++;

		// create .csv line
		char tmpLine[24] = {};
		sprintf(tmpLine, "%lu, %lu\n", allDataTime, allDataFreq);
		nf_add_line(tmpLine);
	}
	free(allData);


	led_off();
	nf_publish();
}


// function which measures at multiple channels and frequencies at the same time
void measure_function_6(){
	uint16_t nrData = 125;
	uint32_t intervall = 50;
	// set multiplexer for sChannel
	uint8_t sChannel = 0;
	errGlob = nm_set(0, sChannel);
	errGlob = nm_set(1, sChannel);

	uint32_t tmpFreq = 0;

	// init fdc
	errGlob = nc_init();

	// create dataPtr
	uint32_t *allData = (uint32_t*)malloc(nrData * 2 * sizeof(uint32_t));
	uint32_t *allDataPtr = allData;

	// start both timer interrupts
	timer_start(intervall);
	uint16_t counter = 0;
	for(counter = 0; counter < nrData; counter++){
		for(sChannel = 0; sChannel < 4; sChannel++){
			// so interrupt knows intervall is too fast
			currentlyMeasuring = 1;

			// set multiplexer
			errGlob = nm_set(0, sChannel);
			errGlob = nm_set(1, sChannel);

			// stop measurement after button press
			if(stpMeasurement){
				break;
			}

			tmpFreq = 0;
			errGlob = nc_get_freq(&tmpFreq, sChannel);

			*allDataPtr = milliSeconds;
			allDataPtr++;
			*allDataPtr = tmpFreq;
			allDataPtr++;

			currentlyMeasuring = 0;
			__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, enable interrupts
		}
	}

	timer_stop();
	if(errGlob){
		red_led();
	}
	else{
		green_led();
	}

	// create csv file
	char *title = "time [ms], frequency data\n";
	nf_init(title);

	allDataPtr = allData;
	for(counter = 0; counter < nrData; counter++){
		uint32_t allDataTime[4] = {};
		uint32_t allDataFreq[4] = {};
		for(sChannel = 0; sChannel < 4; sChannel++){
			allDataTime[sChannel] = *allDataPtr;
			allDataPtr++;
			allDataFreq[sChannel] = *allDataPtr;
			allDataPtr++;
		}

		// create .csv line
		char tmpLine[24] = {};
		sprintf(tmpLine, "%lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu\n", allDataTime[0], allDataFreq[0], allDataTime[1], allDataFreq[1], allDataTime[2], allDataFreq[2], allDataTime[3], allDataFreq[3]);
		nf_add_line(tmpLine);
	}
	free(allData);


	led_off();
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
		errGlob = 20;
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
