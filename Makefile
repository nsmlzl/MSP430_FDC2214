TARGET = main.c
INCLUDES = includes/nsi2c.c includes/nsi2c.h


GCC_DIR =  ~/ti/msp430_gcc/bin
SUPPORT_FILE_DIRECTORY = ~/ti/msp430_gcc/include

DEVICE  = msp430f2274
CC      = $(GCC_DIR)/msp430-elf-gcc
GDB     = $(GCC_DIR)/msp430-elf-gdb
UPLOAD	=	mspdebug

CFLAGS = -I $(SUPPORT_FILE_DIRECTORY) -mmcu=$(DEVICE) -O0 -g
LFLAGS = -L $(SUPPORT_FILE_DIRECTORY)

SOURCES = $(TARGET) $(INCLUDES)
OBJECTS = $(SOURCES:.c=.o)

all: ${OBJECTS}
	$(CC) $(CFLAGS) $(LFLAGS) $? -o main.out

program: all
	$(UPLOAD) rf2500 erase "prog main.out"

debug: all
	$(GDB) main.out -ex "target remote :2000" -ex "load"

clean:
	rm *.out *.o ./*/*.o

msp:
	#$(UPLOAD) rf2500 reset gdb
	$(UPLOAD) rf2500 erase gdb
