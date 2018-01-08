/*
 * nsi2c.h
 *
 * Created: 26.12.2017
 * Author : Niklas Schmelzle
 *
 * TODO:
 *
 */

#ifndef NSI2C_H
#define NSI2C_H

#include <msp430f5529.h>

void nsi_init(unsigned char i2cAddress);
void nsi_transmit(unsigned char i2cAddress, unsigned char byteCtr, unsigned char *transmitData);
void nsi_receive(unsigned char i2cAddress, unsigned char byteCtr, unsigned char *receiveData);

#endif
