This is the source code of the MMDVM firmware that supports D-Star, DMR, System Fusion, P25, NXDN, M17, POCSAG, AX.25, and FM modes.

It runs on the Arduino Due, the ST-Micro STM32F1xxx, STM32F4xxx and STM32F7xxx processors, as well as the Teensy 3.1/3.2/3.5/3.6. What these platforms have in common is the use of an ARM Cortex-M3, M4, or M7 processors with a minimum clock speed greater of 70 MHz, and access to at least one analogue to digital converter, one digital to analogue converter, as well as a number of GPIO pins.

In order to build this software for the Arduino Due, you will need to edit a file within the Arduino GUI and that is detailed in the BUILD.txt file. The STM32 support is supplied via the ARM GCC compiler. The Teensy support uses Teensyduino.

This software is licenced under the GPL v2 and is intended for amateur and educational use only. Use of this software for commercial purposes is strictly forbidden.
