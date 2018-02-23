#include "neeprom.h"

const uint8_t EEPROMADDR = 0x50;


// write array saveData to EEPROM at address reg1, reg2
// this function stays on the same EEPROM page (don't try to change the 8th bit)
int16_t ne_write(uint8_t reg1, uint8_t reg2, uint8_t nrBytes, uint8_t *saveData){
	const uint8_t dataBytes = nrBytes + 2;
	uint8_t *data = (uint8_t *) malloc(dataBytes * sizeof(uint8_t));
	uint8_t *dataPtr = data;
	// save register addresses to front of array
	*dataPtr = reg1;
	dataPtr++;
	*dataPtr = reg2;
	dataPtr++;
	// put data behind it
	uint16_t i = 0;
	for(i = 0; i < nrBytes; i++){
		*dataPtr = *saveData;
		dataPtr++;
		saveData++;
	}
	// send array to EEPROM
	int16_t returnValue = ni_transmit(EEPROMADDR, nrBytes + 2, data);
	free(data);
	// wait for EEPROM to finish writing, or timeout to occur
	if(ne_wait()) return 2;
	else return returnValue;
}


// intelligent function that handles the problem with multiple page writing to EEPROM
// nrBytes can be huge
// returns 3 if error occurs
int16_t ne_intel_write(uint8_t reg1, uint8_t reg2, uint16_t nrBytes, uint8_t *saveData){
	while(nrBytes){
		// how much space is left in current page (last seven bits)?
		uint16_t curByteCtr = 128 - (0x7F & reg2);

		// more pages will come
		if(nrBytes > curByteCtr){
			// return 3 if error occurs
			if(ne_write(reg1, reg2, curByteCtr, saveData)) return 3;
			// getting data ready for next round
			if(reg2 & 0x80){
				// 8th bit of reg2 already set, so increase in reg1
				reg1++;
				reg2 = 0x0;
			}
			else{
				// setting 8th bit of reg2
				reg2 = 0x80;
			}
			nrBytes -= curByteCtr;
			saveData += curByteCtr;
		}

		// sending last data set
		else{
			// return 3 if error occurs
			if(ne_write(reg1, reg2, nrBytes, saveData)) return 3;
			nrBytes = 0;
		}
	}
	return 0;
}


// write single byte saveData to EEPROM at address reg1, reg2
int16_t ne_single_write(uint8_t reg1, uint8_t reg2, uint8_t saveData){
	return ne_write(reg1, reg2, 1, &saveData);
}


// read nrBytes elements into readData (as an array) from EEPROM at reg1, reg2
// this function stays on the same EEPROM page (don't try to change the 8th bit)
int16_t ne_read(uint8_t reg1, uint8_t reg2, uint8_t nrBytes, uint8_t *readData){
	uint8_t address[2] = {reg1, reg2};
	return ni_transmit_receive(EEPROMADDR, 2, address, nrBytes, readData);
}


// intelligent function to read data from multiple EEPROM pages (see ne_intel_write)
// nrBytes can be huge
// returns 3 if error occurs
int16_t ne_intel_read(uint8_t reg1, uint8_t reg2, uint16_t nrBytes, uint8_t *readData){
	while(nrBytes){
		// how much space is left in current page (last seven bits)?
		uint16_t curByteCtr = 128 - (0x7F & reg2);

		// more pages will come
		if(nrBytes > curByteCtr){
			// return 3 if error occurs
			if(ne_read(reg1, reg2, curByteCtr, readData)) return 3;
			// getting data ready for next round
			if(reg2 & 0x80){
				// 8th bit of reg2 already set, so increase in reg1
				reg1++;
				reg2 = 0x0;
			}
			else{
				// setting 8th bit of reg2
				reg2 = 0x80;
			}
			nrBytes -= curByteCtr;
			readData += curByteCtr;
		}

		// reading last data set
		else{
			// return 3 if error occurs
			if(ne_read(reg1, reg2, nrBytes, readData)) return 3;
			nrBytes = 0;
		}
	}
	return 0;

}


// function checks if EEPROM is ready for communication
// when eeprom ready -> returns 1, when not -> 0
int16_t ne_ready(){
	return ni_slave_present(0x50);
}


// function blocks as long EEPROM isn't ready
// returns 1 if device doesn't answer until timeout
int16_t ne_wait(){
	// check if EEPROM is ready
	uint16_t timeOut = 0;
	while(!ne_ready()){
		timeOut++;
		__delay_cycles(1000);
		// check for timeout
		if(timeOut > 20){
			return 1;
		}
	}
	return 0;
}
