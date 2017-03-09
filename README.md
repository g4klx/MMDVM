This is the source code of the MMDVM firmware that supports D-Star, DMR, System Fusion and P25 modes.

It runs on the Arduino Due, the ST-Micro STM32F407-DISCO and STM32F446-NUCLEO, as well as the Teensy 3.1/3.2/3.5/3.6. What these platforms have in common is the use of an ARM Cortex-M3 or M4 processor with a clock speed greater than 70 MHz, and access to at least one analogue to digital converter and one digital to analogue converter. A Cortex-M7 processor is also probably adequate.

In order to build this software for the Arduino Due, you will need to edit a file within the Arduino GUI and that is detailed in the BUILD.txt file. The STM32 support is being supplied via the Coocox IDE with ARM GCC. The Teensy support is via the latest beta of Teensyduino.

This software is licenced under the GPL v2 and is intended for amateur and educational use only. Use of this software for commercial purposes is strictly forbidden.
