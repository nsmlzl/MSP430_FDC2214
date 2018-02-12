#! /usr/local/bin/bash
cd ./Debug/
diskutil umount /dev/disk2
if /Applications/ti/ccsv7/utils/bin/gmake -k -j 3 all -O; then 
	if mspdebug tilib erase "prog FDC2214.out"; then
		echo "MSP430 programmed :)"
	else
		echo "Error: Programming MSP430"
	fi
else
	echo "Error: Compile!"
fi
