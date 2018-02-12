#! /usr/local/bin/bash
cd ./Debug/
diskutil umount /dev/disk2
if /Applications/ti/ccsv7/utils/bin/gmake -k -j 3 all -O; then 
	msp430-elf-gdb FDC2214.out -ex "target remote :2000" -ex "load"
else
	echo "Error: Compile!"
fi
