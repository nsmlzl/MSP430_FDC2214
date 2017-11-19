OBJECTS=main.o i2c.o

GCC_DIR =  ~/ti/msp430_gcc/bin
SUPPORT_FILE_DIRECTORY = ~/ti/msp430_gcc/include

DEVICE  = msp430f2274
CC      = $(GCC_DIR)/msp430-elf-gcc
GDB     = $(GCC_DIR)/msp430-elf-gdb
UPLOAD	=	mspdebug

CFLAGS = -I $(SUPPORT_FILE_DIRECTORY) -mmcu=$(DEVICE) -O0 -g
LFLAGS = -L $(SUPPORT_FILE_DIRECTORY)

all: ${OBJECTS}
	$(CC) $(CFLAGS) $(LFLAGS) $? -o main.out

program: all
	$(UPLOAD) rf2500 "prog main.out"

debug: all
	$(GDB) main.out

clean:
	rm *.out *.o
