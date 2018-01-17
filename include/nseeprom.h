/*
 * nseeprom.h
 *
 * Created: 09.01.2018
 * Author : Niklas Schmelzle
 *
 * TODO:
 * - improve nse_write function (copying, etc) with pointers
 *
 */

#ifndef NSEEPROM_H
#define NSEEPROM_H

#include <msp430f5529.h>
#include <inttypes.h>
#include "nsi2c.h"

int16_t nse_write(uint8_t register1, uint8_t register2, uint8_t tmpByteCtr, uint8_t *tmpData);
int16_t nse_intel_write(uint8_t register1, uint8_t register2, uint16_t tmpByteCtr, uint8_t *tmpData);
int16_t nse_single_write(uint8_t register1, uint8_t register2, uint8_t tmpData);
int16_t nse_read(uint8_t register1, uint8_t register2, uint8_t tmpByteCtr, uint8_t *readData);
int16_t nse_intel_read(uint8_t register1, uint8_t register2, uint16_t tmpByteCtr, uint8_t *readData);
int16_t nse_ready();
int16_t nse_wait();

#endif
