/*
 * nseeprom.h
 *
 * Created: 09.01.2018
 * Author : Niklas Schmelzle
 *
 * TODO:
 * - improve nse_write function (copying, etc) with pointers
 * - improve nse_write and nse_read:
 *   currently a big problem, if we work with data not only on a single page (128 byte)
 *
 */

#ifndef NSEEPROM_H
#define NSEEPROM_H

#include <msp430f5529.h>
#include <inttypes.h>
#include "nsi2c.h"

uint16_t nse_write(uint8_t register1, uint8_t register2, uint8_t tmpByteCtr, uint8_t *tmpData);
uint16_t nse_single_write(uint8_t register1, uint8_t register2, uint8_t tmpData);
uint16_t nse_read(uint8_t register1, uint8_t register2, uint8_t tmpByteCtr, uint8_t *readData);

#endif
