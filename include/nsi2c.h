/*
 * nsi2c.h
 *
 * Created: 26.12.2017
 * Author : Niklas Schmelzle
 *
 * TODO:
 * -repeated start
 *
 */

#ifndef NSI2C_H
#define NSI2C_H

#include <msp430f5529.h>

// void nsi_init(unsigned char i2cAddr);
int nsi_transmit(unsigned char i2cAddr, unsigned char tmpByteCtr, unsigned char *tmpTXData);
int nsi_receive(unsigned char i2cAddr, unsigned char tmpByteCtr, unsigned char *tmpRXData);
int nsi_transmit_receive(unsigned char i2cAddr, unsigned char txByteCtr, unsigned char *tmpTXData, unsigned char rxByteCtr, unsigned char *tmpRXData);

#endif
