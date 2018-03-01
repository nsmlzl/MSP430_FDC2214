#include "nfdc.h"

void nc_set_interrupt_port();

const uint8_t FDCADDR = 0x2A;


uint8_t nc_init(){
	uint8_t err = 0;

	// reset device
	uint8_t resetDevice[3] = {0x1C, 0x80, 0x00};
	err += ni_transmit(FDCADDR, 3, resetDevice);

	// channel 3: 8 MHz
	// setting chx_rcount (to max = 0xFFFF)
	uint8_t rcount[3] = {0x0B, 0xFF, 0xFF};
	err += ni_transmit(FDCADDR, 3, rcount);
	// setting chx_settlcount (to 0x400 from GUI; alot higher than needed)
	uint8_t settlecount[3] = {0x13, 0x04, 0x00};
	err += ni_transmit(FDCADDR, 3, settlecount);
	// setting clock divider (chx_fin_sel = 0b01, chx_fref_divider = 0b1)
	uint8_t clockdivider[3] = {0x17, 0x10, 0x01};
	err += ni_transmit(FDCADDR, 3, clockdivider);
	// setting idrive (ch3 = 12, 1.2V < A < 1.8V)
	uint8_t idrive[3] = {0x21, 0x60, 0x00};
	err += ni_transmit(FDCADDR, 3, idrive);

	// setting status register (amplitude warnings and interrupt for new data)
	uint8_t statusconf[3] = {0x19, 0x18, 0x1};
	err += ni_transmit(FDCADDR, 3, statusconf);
	// setting mux_config (autoscen_en = 0, rr_sequence = b00, deglitch = b101
	// and reserved values?)
	// -> single channel measurement
	uint8_t muxconf[3] = {0x1B, 0x02, 0x0D};
	err += ni_transmit(FDCADDR, 3, muxconf);
	// setting config (active_chann = b11, sleepmode = 1, sensor_activate_sel = 1,
	// ref_clk_src = 1 and reserved values?)
	// -> single channel measurement for channel 3
	uint8_t config[3] = {0x1A, 0x3E, 0x01};
	err += ni_transmit(FDCADDR, 3, config);

	// read status to clear int
	uint8_t statusAddr = 0x18;
	uint8_t statusRx[2] = {};
	// err += ni_transmit_receive(FDCADDR, 1, &statusAddr, 2, statusRx);

	return err;
}


// read measured frequency data from fdc for channel.
// with this data the capacity can be calculated
uint8_t nc_get_data(uint32_t *dataPtr, uint8_t channel){
	uint8_t err = 0;

	// read frequency data from channel
	// (each channel has its data saved to two registers)
	uint8_t dataRx[4] = {};
	uint8_t *dataRxPtr = dataRx;
	uint8_t dataReg = 0;
	for(dataReg = channel * 2; dataReg < channel * 2 + 2; dataReg++){
		err += ni_transmit_receive(FDCADDR, 1, &dataReg, 2, dataRxPtr);
		dataRxPtr += 2;
	}

	*dataPtr = 0;															// init ptr to zero
	// check error flags in data byte for channel
	if(dataRx[0] & 0x30){
		err = 0xFF;
	}
	// use only the last 4 bits (others aren't data)
	dataRx[0] = dataRx[0] & 0xF;

	// combine data for channel (4 byte) to single uint32_t
	uint8_t byte = 0;
	for(byte = 0; byte < 4; byte++){
		*dataPtr |= (uint32_t) dataRx[byte] << ((3 - byte) * 8);
	}

	return err;
}


// measure capacity on channel
uint8_t nc_get_capacity(uint32_t *capacity, uint8_t channel){
	uint8_t err = 0;
	// activate interrupt port
	nc_set_interrupt_port();

	// start measurement
	uint8_t upperBit = 0x1E | channel << 6;
	uint8_t config[3] = {0x1A, upperBit, 0x01};
	err += ni_transmit(FDCADDR, 3, config);

	__bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, enable interrupts

	// get data
	uint32_t freqData = 0;
	err += nc_get_data(&freqData, channel);

	// calculate capacity
	double dbFreq = 40000000 * (double) freqData / pow(2, 28);
	double dbCapacity = 1.0 / (18.41 * pow(10, -6)) / pow(dbFreq * 2 * 3.14159265358979323846, 2);
	// round value to 10^-15, print as femto Fahrrad
	*capacity = (uint32_t) nearbyint(dbCapacity * pow(10, 15));

	// put FDC to sleep
	config[1] = 0x3E;
	err += ni_transmit(FDCADDR, 3, config);

	return err;
}


// set interrupt for P1.3
void nc_set_interrupt_port(){
	// set P1.3 to input
	P1DIR &= ~BIT3;
	// set etch select for P1.3 (high to low)
	P1IES |= BIT3;
	// set interrupt for P1.3
	P1IE |= BIT3;
	// reset old interrupt for P1.3
	P1IFG &= ~(BIT3);
}


// interrupt function for port1
// (we use P1.3 especially)
#pragma vector = PORT1_VECTOR
__interrupt void Port_1(){
	P1IE &= ~BIT3;														// unset interrupt
	__bic_SR_register_on_exit(LPM0_bits);
}
