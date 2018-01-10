#include "nsi2c.h"

volatile uint8_t *txData;
volatile uint8_t *rxData;
volatile uint8_t byteCtr;
volatile int16_t nackOccured;
int16_t repeatedStart;


// init usci i2c module
void nsi_init(uint8_t i2cAddr){
	// init I2C
	P3SEL |= 0x03;                            // Assign I2C pins to USCI_B0
	UCB0CTL1 |= UCSWRST;                      // Enable SW reset
	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
	UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
	UCB0BR0 = 12;                             // fSCL = SMCLK/12 = ~100kHz
	UCB0BR1 = 0;
	UCB0I2CSA = i2cAddr;                   		// Set slave address
	UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation

	UCB0IE = 0;																// unset Interrupts
	nackOccured = 0;													// reset NACK
	repeatedStart = 0;
}


// transmit data
int16_t nsi_transmit(uint8_t i2cAddr, uint8_t tmpByteCtr, uint8_t *tmpTXData){
	nsi_init(i2cAddr);
	// save locals to global variables (for interrupts)
	byteCtr = tmpByteCtr;
	txData = tmpTXData;

	uint8_t tmpUCB0IE = UCB0IE;								// save interrupt settings
	UCB0IE |= UCTXIE + UCNACKIE; 							// set I2C interrupts (transmit, NACK)

	UCB0CTL1 |= UCTR + UCTXSTT;             	// I2C TX, start condition
	// only enter LPM0, if stop or nack signal wasn't send
	if(!(UCB0CTL1 & UCTXSTP) && !nackOccured){
		// data transmit
		__bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, enable interrupts
		// data was transmitted
	}
	// wait for clearing of stop flag
	while(UCB0CTL1 & UCTXSTP);

	UCB0IE = tmpUCB0IE;												// unset I2C interrupts
	return nackOccured;
}


// receive data
int16_t nsi_receive(uint8_t i2cAddr, uint8_t tmpByteCtr, uint8_t *tmpRXData){
	nsi_init(i2cAddr);
	// connect local variables to globals
	byteCtr = tmpByteCtr;
	rxData = tmpRXData;

	uint8_t tmpUCB0IE = UCB0IE;								// save interrupt settings
	UCB0IE |= UCRXIE + UCNACKIE;							// set I2C interrupts (receive, NACK)

	UCB0CTL1 &= ~UCTR;												// set receive mode
	UCB0CTL1 |= UCTXSTT;											// start i2c
	// if only one byte: send stop signal immediately after start flag is cleared
	// this way, only one byte gets send by the slave
	// don't do this, if slave send a NACK, just wait for the clearing of stop flag
	if(byteCtr == 1){
		while((UCB0CTL1 & UCTXSTT) && !nackOccured);
		if(!nackOccured) UCB0CTL1 |= UCTXSTP;
	}
	// only enter LPM0, if stop or nack signal wasn't send
	if(!(UCB0CTL1 & UCTXSTP) && !nackOccured){
		// receive data
		__bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, enable interrupts
		// data was received
	}
	// wait for clearing of stop flag
	while(UCB0CTL1 & UCTXSTP);								// check if stop signal was send

	UCB0IE = tmpUCB0IE;												// unset I2C interrupts
	return nackOccured;
}


// function first transfers tmpTXData and receives tmpRXData after repeated start
int16_t nsi_transmit_receive(uint8_t i2cAddr, uint8_t txByteCtr, uint8_t *tmpTXData, uint8_t rxByteCtr, uint8_t *tmpRXData){
	// transmit
	nsi_init(i2cAddr);
	repeatedStart = 1;
	// save locals to global variables (for interrupts)
	byteCtr = txByteCtr;
	txData = tmpTXData;

	uint8_t tmpUCB0IE = UCB0IE;								// save interrupt settings
	UCB0IE |= UCTXIE + UCNACKIE; 							// set I2C interrupts (transmit, NACK)

	UCB0CTL1 |= UCTR + UCTXSTT;             	// I2C TX, start condition
	// only enter LPM0, if stop or nack signal wasn't send
	if(!(UCB0CTL1 & UCTXSTP) && !nackOccured){
		// data transmit
		__bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, enable interrupts
		// data was transmitted
	}

	if(!nackOccured){													// only try receive, if no NACK was received
		// receive
		repeatedStart = 0;
		// connect local variables to globals
		byteCtr = rxByteCtr;
		rxData = tmpRXData;

		UCB0IE = tmpUCB0IE;											// unset I2C interrupts
		UCB0IE |= UCRXIE + UCNACKIE;						// set I2C interrupts (receive, NACK)

		UCB0CTL1 &= ~UCTR;											// set receive mode
		UCB0CTL1 |= UCTXSTT;										// start i2c
		// if only one byte: send stop signal immediately after start flag is cleared
		// this way, only one byte gets send by the slave
		// don't do this, if slave send a NACK, just wait for the clearing of stop flag
		if(byteCtr == 1){
			while((UCB0CTL1 & UCTXSTT) && !nackOccured);
			if(!nackOccured) UCB0CTL1 |= UCTXSTP;
		}
		// only enter LPM0, if stop or nack signal wasn't send
		if(!(UCB0CTL1 & UCTXSTP) && !nackOccured){
			// receive data
			__bis_SR_register(LPM0_bits + GIE);   // Enter LPM0, enable interrupts
			// data was received
		}
		// wait for clearing of stop flag
		while(UCB0CTL1 & UCTXSTP);							// check if stop signal was send

		UCB0IE = tmpUCB0IE;											// unset I2C interrupts
	}
	return nackOccured;
}

// USCI I2C Interrupt
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	switch(__even_in_range(UCB0IV,12)){
		case  0:	break;                        // Vector  0: No interrupts
		case  2:	break;                        // Vector  2: ALIFG
		case  4:																// Vector  4: NACKIFG
							// send stop signal and exit active cpu
							UCB0CTL1 |= UCTXSTP;
							__bic_SR_register_on_exit(LPM0_bits);
							nackOccured = 1;
							break;
		case  6:	break;                        // Vector  6: STTIFG
		case  8:	break;                        // Vector  8: STPIFG
		case 10:                                // Vector 10: RXIFG
							byteCtr--;
							// send stop signal before last byte is transmitted
							if(byteCtr == 1){
								UCB0CTL1 |= UCTXSTP;
							}
							// last byte gets transmitted; exit active cpu
							else if(byteCtr == 0){
								__bic_SR_register_on_exit(LPM0_bits);
							}
							// save last received byte
							*rxData = UCB0RXBUF;
							rxData++;
							break;
		case 12:                           			// Vector 12: TXIFG
							// all bytes were send
							if(byteCtr == 0){
								// send stop signal and exit active cpu
								// when repeated start: start flag is set in function
								if(!repeatedStart) UCB0CTL1 |= UCTXSTP;
								__bic_SR_register_on_exit(LPM0_bits);
							}
							// write single byte to transmit buffer
							else{
								UCB0TXBUF = *txData;
								txData++;
								byteCtr--;
							}
							break;
		default: break;
	}
}
