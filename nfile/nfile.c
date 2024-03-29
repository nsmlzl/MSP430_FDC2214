/* --COPYRIGHT--,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/

#include "nfile.h"

void nf_createMSC();

FATFS fatfs;
FIL file;
const char *comma = ", ";
const char *eol = "\n";


// init for custom csv creation
void nf_init(char *titleStr){
	f_mount(0, &fatfs);

	// remove example file from TI
	char *pathFileToRemove = "data_log.txt";
	f_unlink(pathFileToRemove);

	// create and open .csv file
	f_open(&file, "data.csv", FA_CREATE_ALWAYS | FA_WRITE);
	// put titles on top
	f_puts(titleStr, &file);
}


void nf_add_line(char *line){
	f_puts(line, &file);
}


void nf_publish(){
	f_close(&file);
	f_mount(0, NULL);

	nf_createMSC();
}


// create .csv file from *dataArray and set MSP as MSC device
void nf_createCSV(char *titleStr, uint32_t *dataArray, uint16_t nrRows, uint16_t nrColumns){
	f_mount(0, &fatfs);

	// remove example file from TI
	char *pathFileToRemove = "data_log.txt";
	f_unlink(pathFileToRemove);

	// create and open .csv file
	f_open(&file, "data.csv", FA_CREATE_ALWAYS | FA_WRITE);
	// put titles on top
	f_puts(titleStr, &file);

	uint16_t tmpRows = 0;
	for(tmpRows = 0; tmpRows < nrRows; tmpRows++){
		// print column
		uint16_t tmpColumns = 0;
		for(tmpColumns = 0; tmpColumns < nrColumns; tmpColumns++){
			// convert integer to string
			char intStr[10] = {};
			sprintf(intStr, "%lu", *dataArray);
			// put string into .csv file
			f_puts(intStr, &file);
			dataArray++;
			// if not end of line, put a comma behind element
			if(tmpColumns != nrColumns - 1){
				f_puts(comma, &file);
			}
		}
		// end of line -> eof char
		f_puts(eol, &file);
	}


	f_close(&file);
	f_mount(0, NULL);

	nf_createMSC();
}


// set MSP430 as MSC device
void nf_createMSC(){
	// Minimum Vcore setting required for the USB API is PMM_CORE_LEVEL_2 .
	PMM_setVCore(PMM_CORE_LEVEL_2);

	USBHAL_initPorts();           // Config GPIOS for low-power (output low)
	USBHAL_initClocks(8000000);   // Config clocks. MCLK=SMCLK=FLL=8MHz; ACLK=REFO=32kHz
	USBMSC_initFSE();          // Initialize the file system emulation volume
	USB_setup(TRUE, TRUE); // Init USB & events; if a host is present, connect

	__enable_interrupt();  // Enable interrupts globally

	while (1)
	{
		switch (USB_getConnectionState())
		{
			case ST_ENUM_ACTIVE:
				USBMSC_processFSEBuffer(); // Handle READ/WRITE cmds from the host
				break;

				// These cases are executed while your device is disconnected from
				// the host (meaning, not enumerated); enumerated but suspended
				// by the host, or connected to a powered hub without a USB host
				// present.
			case ST_PHYS_DISCONNECTED:
			case ST_ENUM_SUSPENDED:
			case ST_PHYS_CONNECTED_NOENUM_SUSP:
				__bis_SR_register(LPM3_bits + GIE);
				_NOP();
				break;

				// The default is executed for the momentary state
				// ST_ENUM_IN_PROGRESS.  Usually, this state only last a few
				// seconds.  Be sure not to enter LPM3 in this state; USB
				// communication is taking place here, and therefore the mode must
				// be LPM0 or active-CPU.
			case ST_ENUM_IN_PROGRESS:
			default:;
		}
	}  //while(1)
} //main()

/*
 * ======== UNMI_ISR ========
 */
#if defined(__TI_COMPILER_VERSION__) || (__IAR_SYSTEMS_ICC__)
#pragma vector = UNMI_VECTOR
__interrupt void UNMI_ISR (void)
#elif defined(__GNUC__) && (__MSP430__)
void __attribute__ ((interrupt(UNMI_VECTOR))) UNMI_ISR (void)
#else
#error Compiler not found!
#endif
{
	switch (__even_in_range(SYSUNIV, SYSUNIV_BUSIFG))
	{
		case SYSUNIV_NONE:
			__no_operation();
			break;
		case SYSUNIV_NMIIFG:
			__no_operation();
			break;
		case SYSUNIV_OFIFG:
			UCS_clearFaultFlag(UCS_XT2OFFG);
			UCS_clearFaultFlag(UCS_DCOFFG);
			SFR_clearInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);
			break;
		case SYSUNIV_ACCVIFG:
			__no_operation();
			break;
		case SYSUNIV_BUSIFG:
			// If the CPU accesses USB memory while the USB module is
			// suspended, a "bus error" can occur.  This generates an NMI.  If
			// USB is automatically disconnecting in your software, set a
			// breakpoint here and see if execution hits it.  See the
			// Programmer's Guide for more information.
			SYSBERRIV = 0; //clear bus error flag
			USB_disable(); //Disable
	}
}

//Released_Version_5_20_06_02
