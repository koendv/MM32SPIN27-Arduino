# MM32SPIN27 arduino

Because of cost or supply problems, people look for alternatives to well-established microcontrollers. The [MindMotion MM32SPIN27](https://mindmotion.com.cn/en/development_tools/info.aspx?itemid=3388) is a low-cost Arm Cortex-M0 microcontroller with a pin-out compatible with the STM32F103.  The MM32SPIN27 is an alternative to the STM32F103, if your application

- does not need USB
- does not need VBAT battery-backup
- does not need hardware FPU floating point unit (software floating point is OK)
- does not need EEPROM (storing settings in flash is OK)
- needs only 12 kbyte RAM or less

This is an arduino for MM32SPIN27 and MM32SPIN25 processors.

- MM32SPIN27: 128kbyte flash, 12kbyte ram, 96MHz
- MM32SPIN25:  32kbyte flash, 8kbyte ram, 96MHz

This is modest but sufficient to write small projects.  As a demo project, see the [t12-958 soldering iron controller](https://github.com/koendv/t12-958).

## compiling

- Install arduino IDE
- In __File ⇨ Preferences... ⇨ Additional Board Manager URLs__, put the URL of the _package_seekfree_index.json_ file: ``https://raw.githubusercontent.com/koendv/MM32SPIN27-Arduino/main/package_seekfree_index.json``
- close and restart arduino IDE.
- for hardware division, copy the file libgcc.a: 
``cp mm32-libgcc/libgcc.a ~/.arduino15/packages/SeekFree/tools/xpack-arm-none-eabi-gcc/10.3.1-2.3/lib/gcc/arm-none-eabi/10.3.1/thumb/v6-m/nofp/libgcc.a``
- in the upper left, in __Select other board and port...__ type: __MM32SPIN27__
- in __File ⇨ Open...__ choose the project. Example: __t12-958.ino__
- choose __Sketch... ⇨ Verify/Compile__
- upload firmware using a debugger (H7-Tool, JLink) or using the serial port (pin BOOT0 high).

## development platform

Arduino IDE 2.0 and arduino-cli is used.

### linux

Linux is used as development platform. Files are case-sensitive. For best results, use a debian-based linux.

### windows

To successfully compile on Windows, directories need to be configured as case-sensitive. Else the system does not distinguish between "String.h" and "string.h".

On Windows, arduino stores the MM32SPIN27 files under the directory ``C:\Users\Name\AppData\Local\Arduino15\packages\SeekFree``, where Name is your Windows user name.
To make this directory and subdirectories case sensitive:

- in File Explorer, go to ``C:\Users\Name\AppData\Local\Arduino15\packages``
- compress the SeekFree folder to SeekFree.zip
- delete the SeekFree folder
- create an empty SeekFree folder
- run PowerShell as Administrator and at the command prompt type:
```
Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Windows-Subsystem-Linux
fsutil.exe file setCaseSensitiveInfo "C:\Users\Name\AppData\Local\Arduino15\packages\SeekFree" enable
```
- unzip SeekFree.zip

The SeekFree directory and the files and subdirectories in the SeekFree directory are now case-sensitive.

## hardware  acceleration

MM32SPIN27 is an Arm Cortex-m0 processor. Cortex-M0 has no hardware FPU, no division instruction.
However, MM32SPIN27 has hardware acceleration for 32-bit integer division and remainder, integer square root and crc.

On a small processor like this, if there is hardware support for something, it's there for a reason. Use it.

### hardware division
Standard libgcc on Cortex-M0 uses software integer division. Division in software works, but is slow. Using hardware division speeds up execution noticeably. This software patches libgcc to use MM32 hardware support for 32-bit signed and unsigned integer division. If you wish to use hardware division you have to replace the compiler ``libgcc.a`` by the patched version.  See directory ``mm32-libgcc``

There is a small complication. Hardware division needs the AHB clock to function.  So after replacing libgcc.a the operators ``/`` and ``%`` can not be used before system clock initialization. In particular, _SetSysClock()_ has been patched not to use division ``/`` to calculate PLL settings.

  Other operands, such as ``+ - *`` work normally.

### hardware crc and square root

Support for hardware crc is through the arduino library ``HWCRC``. Only one crc32 algorithm is supported. See ``hwcrc_performance.ino`` for a speed comparison between software crc and hardware crc.

Support for hardware integer square root is the arduino library ``HWSQRT``.

## functions

The following functions were ported to MM32SPIN27:

- attachInterrupt(), detachInterrupt()
- analogRead(), analogReadResolution(), analogSampleTime()
- analogWrite(), analogWriteResolution(), analogWriteFrequency()
- digitalWrite(), digitalRead(), digitalToggle()
- millis(), delay(), delayMicroseconds()

The following libraries are included:

- _FlashStorage_ to store settings in flash
- _HWCRC_ CRC32 in hardware
- _HWSQRT_ integer square root in hardware
- _RTTStream_ Segger RTT
- _Serial_ hardware UART
- _SoftwareWire_ I2C in software
- _SPI_ in hardware

This list was determined in the following way: I wrote a demo project. If something was needed, I added it.

## demo project

As a demo, a [T12 soldering station](https://github.com/koendv/t12-958) was implemented. This is an application with an SPI OLED display, a rotary encoder with pushbutton, analog and digital inputs, and PWM output. Standard arduino libraries were used for display, task scheduler, PID controller, and temperature sensors.

## free memory

The linker checks there is enough free memory. If you get the following error, there is not enough free memory.

```
arm-none-eabi/bin/ld: region `RAM' overflowed by 123 bytes
collect2: error: ld returned 1 exit status
```
To solve this problem, either

- rewrite the program to use less memory, or
- lower the amount of free memory needed. In ``boards.txt``, change ``MM32SPIN27.build.min_free_ram=3K``

## firmware download

After successful compilation, binaries are in ``/home/UserName/Arduino/ProjectName/build/SeekFree.mm32.MM32SPIN27/ProjectName.ino.elf``

The compiled firmware can then be downloaded to the MM32SPIN27:

- Segger JLink does not explicitly support MM32SPIN27 but MM32L072XX works.
- OpenOCD has an old patch to support the similar [MM32L062](https://sourceforge.net/p/openocd/mailman/message/37388746/)
- [H7-Tool](https://www.armfly.com/product/H7-TOOL/H7-TOOL.shtml) supports many "Made in Asia" processors, including MM32. Includes an RTT Viewer. H7-Tool is an [open-source](https://github.com/armfly/H7-TOOL_STM32H7_App) commercial product.
- firmware [download via serial port](mm32/doc/isp).

## compiler version

The compilers from the [xpack project](https://xpack.github.io/arm-none-eabi-gcc/) are used.

If you wish to change the compiler version: Compiler version is specified twice, once in platform.txt:
```
compiler.path={runtime.tools.xpack-arm-none-eabi-gcc-10.3.1-1.2.3.path}/bin/
```
and once in
package_seekfree_index.json:
```
          "toolsDependencies": [
            {
              "packager": "SeekFree",
              "name": "xpack-arm-none-eabi-gcc",
              "version": "10.3.1-2.3"
            },
```

## other processors

This software is for MM32SPIN27.  This software can be adapted to other MindMotion MM32 Cortex-M0 processors.

First, in boards.txt change ram size, flash size, minimum free ram, and see if you can run a small arduino sketch to blink a led. Maybe you are lucky.

- boards.txt: set ram size, flash size, minimum free memory.
- package_seekfree_index.json: add board to  packages->platforms->boards
- check board definition in ``variants`` directory, create new board if necessary
- replace cores/arduino/HAL_Lib with HAL_Lib for the new processor
- replace cores/arduino/MM32SPIN2xx_p with SDK for the new processor
- to add a directory to the include path, in ``platform.txt`` add the path to ``compiler.seekfree.extra_include``

## References
- [MM32SPIN27 datasheet](mm32/doc/pdf/DS_MM32SPIN2x_p_EN.pdf) (English)
- [MM32SPIN27 product page](https://mindmotion.com.cn/en/development_tools/info.aspx?itemid=3388) (English)
- [HAL library from MindMotion](https://mindmotion.com.cn/products/mm32mcu/mm32spin/mm32spin_specific_mcu/mm32spin2x/)
- [SeekFree library for the MM32SPIN27](https://gitee.com/seekfree/MM32SPIN27_Library)
- [ArduinoCore-API](https://github.com/arduino/ArduinoCore-API)
- [mm32_startup](https://github.com/iclite/mm32_startup/)
