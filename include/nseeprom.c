#include "nseeprom.h"

static uint8_t EEPROMADDR = 0x50;


// write array tmpData to EEPROM at address register1, register2
uint16_t nse_write(uint8_t register1, uint8_t register2, uint8_t tmpByteCtr, uint8_t *tmpData){
	uint8_t data[tmpByteCtr + 2];
	// save register addresses to front of array
	data[0] = register1;
	data[1] = register2;
	// put data behind it
	for(uint16_t i = 0; i < tmpByteCtr; i++){
		data[i + 2] = *tmpData;
		tmpData++;
	}
	// send array to EEPROM
	return nsi_transmit(EEPROMADDR, tmpByteCtr + 2, data);
}


// write single byte tmpData to EEPROM at address register1, register2
uint16_t nse_single_write(uint8_t register1, uint8_t register2, uint8_t tmpData){
	return nse_write(register1, register2, 1, &tmpData);
}


// read tmpByteCtr elements into readData (as an array) from EEPROM at register1, register2
uint16_t nse_read(uint8_t register1, uint8_t register2, uint8_t tmpByteCtr, uint8_t *readData){
	uint8_t address[2] = {register1, register2};
	return nsi_transmit_receive(EEPROMADDR, 2, address, tmpByteCtr, readData);
}

