#include "nsi2c.h"

unsigned char i2cData = 0;


void nsi_init(void){
	// init I2C
	P3SEL |= 0x06;                            // Assign I2C pins to USCI_B0
	UCB0CTL1 |= UCSWRST;                      // Enable SW reset
	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
	UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
	UCB0BR0 = 12;                             // fSCL = SMCLK/12 = ~100kHz
	UCB0BR1 = 0;
	UCB0I2CSA = 0x50;                   			// Set slave address
	UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
	// IE2 |= UCB0RXIE;												// Enable RX interrupt
	// IE2 |= UCB0TXIE;
	// TACCTL0 = CCIE;                       	// TAC
	// TACTL = TASSEL_2 + MC_2;              	// SMCLK, contmode

	// transmit data
	UCB0CTL1 |= UCTR + UCTXSTT;             	// I2C TX, start condition
	while(!(IFG2 & UCB0TXIFG));								// wait until data can be written
	UCB0TXBUF = 0x1;
	while(UCB0CTL1 & UCTXSTT);								// wait until start signal is ack
	while(!(IFG2 & UCB0TXIFG));								// wait until data can be written
	UCB0TXBUF = 0x1;
	while(!(IFG2 & UCB0TXIFG));								// wait until data can be written
	UCB0TXBUF = 0x6;
	while(!(IFG2 & UCB0TXIFG));
	UCB0CTL1 |= UCTXSTP;											// stop signal
	while(UCB0CTL1 & UCTXSTP);								// check if stop signal was send

	// wait for write time of EEPROM
	for(int i = 0; i < 160; i++){
		__delay_cycles(1000);
	}

	// init I2C
	P3SEL |= 0x06;                            // Assign I2C pins to USCI_B0
	UCB0CTL1 |= UCSWRST;                      // Enable SW reset
	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
	UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
	UCB0BR0 = 12;                             // fSCL = SMCLK/12 = ~100kHz
	UCB0BR1 = 0;
	UCB0I2CSA = 0x50;                   			// Set slave address
	UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation

	// receaive data
	UCB0CTL1 &= ~UCTR;												// set receive mode
	UCB0CTL1 |= UCTXSTT;											// start i2c
	while(UCB0CTL1 & UCTXSTT);
	unsigned char stat = UCB0STAT;
	while(!(IFG2 & UCB0RXIFG));
	unsigned char test = UCB0RXBUF;
	UCB0CTL1 |= UCTXSTP;											// stop signal
	while(UCB0CTL1 & UCTXSTP);								// check if stop signal was send
}


/* Interrupts
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMERA0_VECTOR
__interrupt void TA0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMERA0_VECTOR))) TA0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	__bic_SR_register_on_exit(CPUOFF);        // Exit LPM0
}

// receive single byte and load it into i2cData
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCIAB0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
	i2cData = UCB0RXBUF;  										// Get received byte
	UCB0CTL1 |= UCTXSTP;                  		// Generate I2C stop condition
	__bic_SR_register_on_exit(CPUOFF);      	// Exit LPM0
}
*/
