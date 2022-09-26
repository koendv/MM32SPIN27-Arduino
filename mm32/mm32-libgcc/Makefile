CC = arm-none-eabi-gcc
AR = arm-none-eabi-gcc-ar
INCDIR = ../cores/arduino
#CFLAGS = -mcpu=cortex-m0 -mthumb -c -Os -w -ffunction-sections -fdata-sections -I${INCDIR}/CMSIS -I${INCDIR}/MM32SPIN2xx_p
CFLAGS = -mcpu=cortex-m0 -mthumb -c -Og -g3 -w -ffunction-sections -fdata-sections -I${INCDIR}/CMSIS -I${INCDIR}/MM32SPIN2xx_p

# libgcc.a copied from ~/.arduino15/packages/SeekFree/tools/xpack-arm-none-eabi-gcc/10.3.1-2.3/lib/gcc/arm-none-eabi/10.3.1/thumb/v6-m/nofp/libgcc.a
LIBGCC=./libgcc.a

SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)

dummy: $(OBJ)
	ar -vrcs libgcc.a $^

all: dummy

clean:
	rm -f $(OBJ)
