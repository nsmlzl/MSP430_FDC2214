/*
 * nfdc.h
 *
 * Created: 2018-02-23
 * Author : Niklas Schmelzle
 *
 * TODO:
 * -> defining exact chx_rcount / chx_settlecount / idrive
 *
 */

#ifndef NFDC_H
#define NFDC_H

#include <msp430f5529.h>
#include <inttypes.h>
#include <math.h>
#include "ni2c.h"

uint8_t nc_init();
uint8_t nc_get_data(uint32_t *dataPtr, uint8_t channel);
uint8_t nc_get_capacity(uint32_t *capacity, uint8_t channel);

#endif
