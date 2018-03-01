#include "nfdc.h"


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

	// setting mux_config (autoscen_en = 1, rr_sequence = b10, deglitch = b101
	// and reserved values?)
	uint8_t muxconf[3] = {0x1B, 0xC2, 0x0D};
	err += ni_transmit(FDCADDR, 3, muxconf);
	// setting config (sleepmode = 0, sensor_activate_sel = 1, ref_clk_src = 1
	// and reserved values?)
	uint8_t config[3] = {0x1A, 0x1E, 0x01};
	err += ni_transmit(FDCADDR, 3, config);

	return err;
}


// read measured frequency data from fdc for all channels.
// with this data the capacity can be calculated
uint8_t nc_get_data(uint32_t *dataPtr){
	uint8_t err = 0;

	// read frequency data from all four channels
	// (each channel has its data saved to two registers)
	uint8_t dataRx[16] = {};
	uint8_t *dataRxPtr = dataRx;
	uint8_t dataReg = 0x0;
	for(dataReg = 0; dataReg < 8; dataReg++){
		err += ni_transmit_receive(FDCADDR, 1, &dataReg, 2, dataRxPtr);
		dataRxPtr += 2;
	}

	// save dataRx into dataPtr for all four channels
	uint8_t channel = 0;
	for(channel = 0; channel < 4; channel++){
		*dataPtr = 0;														// init ptr to zero
		// check error flags in data byte for channel
		if(dataRx[channel * 4] & 0x30){
			err = 0xFF;
		}
		// use only the last 4 bits (others aren't data)
		dataRx[channel * 4] = dataRx[channel * 4] & 0xF;

		// combine data for channel (4 byte) to single uint32_t
		uint8_t byte = 0;
		for(byte = 0; byte < 4; byte++){
			*dataPtr |= (uint32_t) dataRx[channel * 4 + byte] << ((3 - byte) * 8);
		}
		dataPtr++;
	}

	return err;
}
