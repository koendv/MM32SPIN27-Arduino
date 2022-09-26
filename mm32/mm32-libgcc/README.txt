32-bit hardware division for MindMotion MM32. Fast.

Standard libgcc uses software division. Division in software works, but is slow.
This software patches libgcc to use MM32 hardware support for 32-bit signed and unsigned integer division.

COMPILE:
cp ~/.arduino15/packages/SeekFree/tools/xpack-arm-none-eabi-gcc/10.3.1-2.3/lib/gcc/arm-none-eabi/10.3.1/thumb/v6-m/nofp/libgcc.a libgcc.a
make

INSTALL:
cp libgcc.a ~/.arduino15/packages/SeekFree/tools/xpack-arm-none-eabi-gcc/10.3.1-2.3/lib/gcc/arm-none-eabi/10.3.1/thumb/v6-m/nofp/libgcc.a

NOTE:
Start HWDIV clock at system init:
RCC->AHBENR |= RCC_AHBENR_HWDIVEN;

HWDIV needs the AHB clock to function.
So you can not use '/' or '%' before system clock initialization.  '+', '-', '*' work normally.
In particular, SetSysClock() can not use '/' to calculate pll settings. See system_MM32SPIN2xx_p.c.patch

