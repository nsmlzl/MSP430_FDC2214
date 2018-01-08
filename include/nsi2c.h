/*
 * nsi2c.h
 *
 * Created: 26.12.2017
 * Author : Niklas Schmelzle
 *
 * TODO:
 * - receive function with interrupts
 * - NACK flag support
 *
 */

#ifndef NSI2C_H
#define NSI2C_H

#include <msp430f5529.h>

void nsi_init(unsigned char i2cAddress);
void nsi_transmit(unsigned char tmpI2CAddress, unsigned char tmpByteCtr, unsigned char *tmpTXData);
void nsi_receive(unsigned char i2cAddress, unsigned char byteCtr, unsigned char *receiveData);

#endif
