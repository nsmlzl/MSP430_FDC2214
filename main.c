/*
 * main.c
 *
 * Created: 18.11.2017
 * Author : Niklas Schmelzle
 *
 * TODO:
 *
 */

#include <msp430f2274.h>
// #include "includes/TI_USCI_I2C_master.h"
#include "includes/nsi2c.h"

void error();
void redLed();
void greenLed();
void ledOff();


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	// turn LEDs on
	P1DIR = 0x3;
	P1OUT = 0x3;

	// is CC3000 module the problem?
	P3DIR |= 0x0F;

/*
 * Own USCI Library
 */
	nsi_init();

/*
 * USCI Library


	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;
	// enable interrups
	_EINT();
	// initialize USCI
	TI_USCI_I2C_transmitinit(0x50, 0x12);
	// wait for bus to be free
	while (TI_USCI_I2C_notready());

	// check for slave
	if(TI_USCI_I2C_slave_present(0x50) != 0){
		// only green
		P1OUT |= 0x2;
		P1OUT &= ~(1);
	}
	else{
		error();
	}

	// sending 0xA to 0x0101
	unsigned char dataTrans[4] = {0x1, 0x1, 0xA, 0x1};
	TI_USCI_I2C_transmit(3, dataTrans);
	char testTrans = dataTrans[2];

	// reading saved data
	TI_USCI_I2C_transmit(2, dataTrans);
	unsigned char dataRec[5] = {0, 0, 0, 0, 0};
	TI_USCI_I2C_receiveinit(0x50, 0x12);
	while(TI_USCI_I2C_notready());
	TI_USCI_I2C_receive(1, dataRec);
	char testRec = dataRec[0];

	// put CPU to sleep during communication
	LPM0;
	*/


	/*
	while(1){
	}
	*/


	ledOff();
	return 0;
}


// Error function turns red LED on
void error(){
	redLed();
}


// Function turns red LED on
void redLed(){
	P1OUT |= 0x1;
	P1OUT &= ~(1 << 0x1);
}


// Function turns green LED on
void greenLed(){
	P1OUT &= ~(1);
	P1OUT |= 0x2;
}


// Function turns both LED off
void ledOff(){
	P1OUT &= ~(1);
	P1OUT &= ~(1 << 1);
}
