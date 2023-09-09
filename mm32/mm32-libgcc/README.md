# 32-bit hardware division for MindMotion MM32 Cortex-M0

Fast.

Standard libgcc uses software division on Cortex-M0. Division in software works, but is slow.
This software patches libgcc to use MM32 Cortex-M0 hardware support for 32-bit signed and unsigned integer division.

## Compile

Add hardware division to libgcc.a:
```
cd mm32-libgcc
cp ~/.arduino15/packages/SeekFree/tools/xpack-arm-none-eabi-gcc/10.3.1-2.3/lib/gcc/arm-none-eabi/10.3.1/thumb/v6-m/nofp/libgcc.a libgcc.a
cp libgcc.a libgcc.a.ORIG
make
```

## Install

Copy the modified libgcc.a back to the compiler directory. 
```
cp libgcc.a ~/.arduino15/packages/SeekFree/tools/xpack-arm-none-eabi-gcc/10.3.1-2.3/lib/gcc/arm-none-eabi/10.3.1/thumb/v6-m/nofp/libgcc.a
```

## Startup

Hardware division on MM32  Cortex-M0 requires a running AHB clock, and HWDIV clock enabled.

Once libgcc.a has been patched to use hardware division, '/' or '%' cannot be used before system clock initialization, because HWDIV hardware division needs a running AHB clock to function. Other operators '+', '-', '*' work normally, as these operators are instructions.

In particular,  the MM32 startup code SetSysClock() has to be patched not to use '/' to calculate system clock pll settings.
Patch MM32 startup code with [system_MM32SPIN2xx_p.c.patch](system_MM32SPIN2xx_p.c.patch). This patch

- calculates system clock pll settings without using '/'.
- starts HWDIV clock once the AHB clock is running,  with:
```
RCC->AHBENR |= RCC_AHBENR_HWDIVEN;
```

_not truncated_
