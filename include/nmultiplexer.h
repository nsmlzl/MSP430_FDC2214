/*
 * nsmultiplexer.h
 *
 * Created: 2018-01-18
 * Author : Niklas Schmelzle
 *
 * TODO:
 *
 */

#ifndef NSMULTIPLEXER_H
#define NSMULTIPLEXER_H

#include <msp430f5529.h>
#include <inttypes.h>
#include "nsi2c.h"

int16_t nsm_set(uint8_t device, uint8_t channel);
int16_t nsm_disable(uint8_t device);

#endif
