#include "nseeprom.h"

static uint8_t EEPROMADDR = 0x50;


// write array tmpData to EEPROM at address register1, register2
// this function stays on the same EEPROM page (don't try to change the 8th bit)
int16_t nse_write(uint8_t register1, uint8_t register2, uint8_t tmpByteCtr, uint8_t *tmpData){
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
	int16_t returnValue = nsi_transmit(EEPROMADDR, tmpByteCtr + 2, data);
	// wait for EEPROM to finish writing, or timeout to occur
	if(nse_wait()) return 2;
	else return returnValue;
}


// intelligent function that handles the problem with multiple page writing to EEPROM
// tmpByteCtr can be huge
// returns 3 if error occurs
int16_t nse_intel_write(uint8_t register1, uint8_t register2, uint16_t tmpByteCtr, uint8_t *tmpData){
	while(tmpByteCtr){
		// how much space is left in current page (last seven bits)?
		uint16_t curByteCtr = 128 - (0x7F & register2);

		// more pages will come
		if(tmpByteCtr > curByteCtr){
			// return 3 if error occurs
			if(nse_write(register1, register2, curByteCtr, tmpData)) return 3;
			// getting data ready for next round
			if(register2 & 0x80){
				// 8th bit of register2 already set, so increase in register1
				register1++;
				register2 = 0x0;
			}
			else{
				// setting 8th bit of register2
				register2 = 0x80;
			}
			tmpByteCtr -= curByteCtr;
			tmpData += curByteCtr;
		}

		// sending last data set
		else{
			// return 3 if error occurs
			if(nse_write(register1, register2, tmpByteCtr, tmpData)) return 3;
			tmpByteCtr = 0;
		}
	}
	return 0;
}


// write single byte tmpData to EEPROM at address register1, register2
int16_t nse_single_write(uint8_t register1, uint8_t register2, uint8_t tmpData){
	return nse_write(register1, register2, 1, &tmpData);
}


// read tmpByteCtr elements into readData (as an array) from EEPROM at register1, register2
// this function stays on the same EEPROM page (don't try to change the 8th bit)
int16_t nse_read(uint8_t register1, uint8_t register2, uint8_t tmpByteCtr, uint8_t *readData){
	uint8_t address[2] = {register1, register2};
	return nsi_transmit_receive(EEPROMADDR, 2, address, tmpByteCtr, readData);
}


// intelligent function to read data from multiple EEPROM pages (see nse_intel_write)
// tmpByteCtr can be huge
// returns 3 if error occurs
int16_t nse_intel_read(uint8_t register1, uint8_t register2, uint16_t tmpByteCtr, uint8_t *readData){
	while(tmpByteCtr){
		// how much space is left in current page (last seven bits)?
		uint16_t curByteCtr = 128 - (0x7F & register2);

		// more pages will come
		if(tmpByteCtr > curByteCtr){
			// return 3 if error occurs
			if(nse_read(register1, register2, curByteCtr, readData)) return 3;
			// getting data ready for next round
			if(register2 & 0x80){
				// 8th bit of register2 already set, so increase in register1
				register1++;
				register2 = 0x0;
			}
			else{
				// setting 8th bit of register2
				register2 = 0x80;
			}
			tmpByteCtr -= curByteCtr;
			readData += curByteCtr;
		}

		// reading last data set
		else{
			// return 3 if error occurs
			if(nse_read(register1, register2, tmpByteCtr, readData)) return 3;
			tmpByteCtr = 0;
		}
	}
	return 0;

}


// function checks if EEPROM is ready for communication
// when eeprom ready -> returns 1, when not -> 0
int16_t nse_ready(){
	return nsi_slave_present(0x50);
}


// function blocks as long EEPROM isn't ready
// returns 1 if device doesn't answer until timeout
int16_t nse_wait(){
	// check if EEPROM is ready
	uint16_t timeOut = 0;
	while(!nse_ready()){
		timeOut++;
		__delay_cycles(1000);
		// check for timeout
		if(timeOut > 20){
			return 1;
		}
	}
	return 0;
}
