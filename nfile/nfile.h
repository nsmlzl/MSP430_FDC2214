/*
 * nfile.h
 * wrapper function for publishing data array as .csv file, later the
 * memory of the msp430 is accessible as MSC device.
 *
 * void nf_createCSV(...)
 *
 * *titleStr: title of .csv file
 * *dataArray: from matrix[y][x]
 * nrRows: number of rows
 * nrColumns: number of columns
 *
 *
 * Created: 2018-02-12
 * Author : Niklas Schmelzle
 *
 * closely based on M1_FileSystemEmulation from TI USB-API
 *
 * TODO:
 *
 */

#ifndef NFILE_H
#define NFILE_H


#include <string.h>
#include <stdio.h>

#include "nfile/driverlib/MSP430F5xx_6xx/driverlib.h"

#include "USB_config/descriptors.h"
#include "USB_API/USB_Common/device.h"
#include "USB_API/USB_Common/usb.h"

#include "USB_app/mscFse.h"
#include "USB_app/FatFs/ff.h"

/*
 * NOTE: Modify hal.h to select a specific evaluation board and customize for
 * your own board.
 */
#include "hal.h"


// three functions instead for nf_createCSV()
void nf_init(char *titleStr);
void nf_add_line(char *line);
void nf_publish();

// all in one function
void nf_createCSV(char *titleStr, uint32_t *dataArray, uint16_t nrRows, uint16_t nrColumns);

#endif
