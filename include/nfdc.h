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
#include "ni2c.h"

uint8_t nc_init();
uint8_t nc_get_data(uint32_t *dataPtr);

#endif
