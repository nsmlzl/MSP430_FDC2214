#! /usr/local/bin/bash
cd ./Debug/
if /Applications/ti/ccsv7/utils/bin/gmake -k -j 3 all -O; then 
	echo "Program Compiled :)"
else
	echo "Error: Compile!"
fi
