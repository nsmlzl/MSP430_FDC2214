/*
 * nfile.h
 *
 * Created: 2018-02-12
 * Author : Niklas Schmelzle
 *
 * TODO:
 *
 */


/*
 * ======== main.c ========
 * Mass Storage with File System Emulation.
 *
 * This example demonstrates usage in which there is no file system software.
 * Instead, a FAT volume is "emulated".  Also, the entire contents of the volume
 * are not stored inside the MSP430 flash; only few blocks are
 * emulated, including the Master Boot Record, File Allocation Table, and
 * Root Directory.
 *
 +----------------------------------------------------------------------------+
 * Please refer to the Examples Guide for more details.
 *----------------------------------------------------------------------------*/

#ifndef NFILE_H
#define NFILE_H


#include <string.h>

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


void nTestfile();

#endif
