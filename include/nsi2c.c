#include "nsi2c.h"

volatile unsigned char *txData;
volatile unsigned char *rxData;
volatile unsigned char byteCtr;


// init usci i2c module
void nsi_init(unsigned char i2cAddress){
	// init I2C
	P3SEL |= 0x03;                            // Assign I2C pins to USCI_B0
	UCB0CTL1 |= UCSWRST;                      // Enable SW reset
	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
	UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
	UCB0BR0 = 12;                             // fSCL = SMCLK/12 = ~100kHz
	UCB0BR1 = 0;
	UCB0I2CSA = i2cAddress;                   // Set slave address
	UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
	// IE2 |= UCB0RXIE;												// Enable RX interrupt
	// IE2 |= UCB0TXIE;
	// TACCTL0 = CCIE;                       	// TAC
	// TACTL = TASSEL_2 + MC_2;              	// SMCLK, contmode
	UCB0IE = 0;
}


// transmit data
void nsi_transmit(unsigned char tmpI2CAddress, unsigned char tmpByteCtr, unsigned char *tmpTXData){
	nsi_init(tmpI2CAddress);
	int ucb0ieI = UCB0IE;
	// UCB0IE |= UCTXIE; // 						// set I2C interrupts (transmit and stop)
	byteCtr = tmpByteCtr;
	txData = tmpTXData;

	UCB0CTL1 |= UCTR + UCTXSTT;             	// I2C TX, start condition

	for(byteCtr = tmpByteCtr; byteCtr > 0; byteCtr--){
		int status = UCB0STAT;
		// __bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, enable interrupts
		while(!(UCB0IFG & UCTXIFG));
		UCB0TXBUF = *txData;
		txData++;
	}
	/*
	while(!(UCB0IFG & UCTXIFG));
	// __bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, enable interrupts
	UCB0TXBUF = *txData;
	txData++;
	// while(UCB0CTL1 & UCTXSTT);
	while(!(UCB0IFG & UCTXIFG));
	// __bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, enable interrupts
	UCB0TXBUF = *txData;
	txData++;
	while(!(UCB0IFG & UCTXIFG));
	// __bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, enable interrupts
	UCB0TXBUF = *txData;
	txData++;
	*/

	while(!(UCB0IFG & UCTXIFG));
	// __bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, enable interrupts
	UCB0CTL1 |= UCTXSTP;
	while(UCB0CTL1 & UCTXSTP);

	UCB0IE = ucb0ieI;													// unset I2C interrupts
}


// transmit data
void nsi_receive(unsigned char i2cAddress, unsigned char byteCtr, unsigned char *receiveData){
	nsi_init(i2cAddress);

	UCB0CTL1 &= ~UCTR;												// set receive mode
	UCB0CTL1 |= UCTXSTT;											// start i2c
	while(UCB0CTL1 & UCTXSTT);
	unsigned char stat = UCB0STAT;
	while(!(UCB0IFG & UCRXIFG));
	unsigned char receiveTest = 0;
	receiveTest = UCB0RXBUF;
	UCB0CTL1 |= UCTXSTP;											// stop signal
	while(UCB0CTL1 & UCTXSTP);								// check if stop signal was send
}

/*
// Interrupts
// USCI_B0 Data ISR
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	switch(__even_in_range(UCB0IV,12))
	{
		case  0: break;                           // Vector  0: No interrupts
		case  2: break;                           // Vector  2: ALIFG
		case  4: break;                           // Vector  4: NACKIFG
		case  6:                           // Vector  6: STTIFG
						 // UCB0IFG &= ~UCSTTIFG;					// clear start flag
						 break;
		case  8:                           // Vector  8: STPIFG
						 // UCB0IFG &= ~UCSTPIFG;						// clear stop flag
						 // __bic_SR_register_on_exit(LPM0_bits);   // Exit active CPU
						 break;
		case 10:                                  // Vector 10: RXIFG
						 // UCB0IFG &= ~UCRXIFG;
						 // RXData = UCB0RXBUF;                     // Get RX data
						 // __bic_SR_register_on_exit(LPM0_bits);   // Exit LPM0
						 break;
		case 12:                           // Vector 12: TXIFG
						 UCB0IFG &= ~UCTXIFG;
						 __bic_SR_register_on_exit(LPM0_bits);   // Exit active CPU
								if(byteCtr == 0){												// all bytes were send
								UCB0CTL1 |= UCTXSTP;									// send stop bit
								__bic_SR_register_on_exit(LPM0_bits);   // Exit active CPU
								}
								else{																		// write single byte to transmit-buffer
								UCB0TXBUF = *txData;
								txData++;
								byteCtr--;
								__bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, enable interrupts
								}
						 break;
		default: break;
	}
}
*/
/*
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
