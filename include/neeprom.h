/*
 * neeprom.h
 *
 * Created: 2018-01-09
 * Author : Niklas Schmelzle
 *
 * TODO:
 *
 */

#ifndef NEEPROM_H
#define NEEPROM_H

#include <msp430f5529.h>
#include <inttypes.h>
#include <stdlib.h>
#include "ni2c.h"

int16_t ne_write(uint8_t reg1, uint8_t reg2, uint8_t nrBytes, uint8_t *saveData);
int16_t ne_intel_write(uint8_t reg1, uint8_t reg2, uint16_t nrBytes, uint8_t *saveData);
int16_t ne_single_write(uint8_t reg1, uint8_t reg2, uint8_t saveData);
int16_t ne_read(uint8_t reg1, uint8_t reg2, uint8_t nrBytes, uint8_t *readData);
int16_t ne_intel_read(uint8_t reg1, uint8_t reg2, uint16_t nrBytes, uint8_t *readData);
int16_t ne_ready();
int16_t ne_wait();

#endif
