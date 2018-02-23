#include "nmultiplexer.h"


// set multiplexer device to use channel channel
int16_t nm_set(uint8_t device, uint8_t channel){
	uint8_t i2cAddr = 0x48 + device;
	// set enable (forth) bit plus channel name
	uint8_t byte = 0x8 + channel;
	return ni_transmit(i2cAddr, 1, &byte);
}


// disable multiplexer device
int16_t nm_disable(uint8_t device){
	uint8_t i2cAddr = 0x48 + device;
	// unset enable (forth) bit
	uint8_t byte = 0x0;
	return ni_transmit(i2cAddr, 1, &byte);
}
