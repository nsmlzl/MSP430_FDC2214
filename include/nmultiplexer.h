/*
 * nmultiplexer.h
 *
 * Created: 2018-01-18
 * Author : Niklas Schmelzle
 *
 * TODO:
 *
 */

#ifndef NMULTIPLEXER_H
#define NMULTIPLEXER_H

#include <msp430f5529.h>
#include <inttypes.h>
#include "ni2c.h"

int16_t nm_set(uint8_t device, uint8_t channel);
int16_t nm_disable(uint8_t device);

#endif
