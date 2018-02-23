/*
 * ni2c.h
 *
 * Created: 2017-12-26
 * Author : Niklas Schmelzle
 *
 * TODO:
 * - timer functions, so if no connection the program doesn't get trapped
 *
 */

#ifndef NI2C_H
#define NI2C_H

#include <msp430f5529.h>
#include <inttypes.h>

void ni_init(uint8_t i2cAddr);
int16_t ni_transmit(uint8_t i2cAddr, uint8_t tmpByteCtr, uint8_t *tmpTXData);
int16_t ni_receive(uint8_t i2cAddr, uint8_t tmpByteCtr, uint8_t *tmpRXData);
int16_t ni_transmit_receive(uint8_t i2cAddr, uint8_t txByteCtr, uint8_t *tmpTXData, uint8_t rxByteCtr, uint8_t *tmpRXData);
int16_t ni_slave_present(uint8_t i2cAddr);

#endif
