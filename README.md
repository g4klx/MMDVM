This is the source code of the MMDVM firmware that supports D-Star, DMR, System Fusion, P25, NXDN, M17, POCSAG, AX.25, and FM modes.

It runs on the Arduino Due, the ST-Micro STM32F4xxx and STM32F7xxx processors, as well as the Teensy 3.5/3.6. What these platforms have in common is the use of an ARM Cortex-M3, M4, or M7 processors with a minimum clock speed greater of 80 MHz, and access to at least one analogue to digital converter, one digital to analogue converter, as well as a number of GPIO pins.

In order to build this software for the Arduino Due, you will need to edit a file within the Arduino GUI and that is detailed in the BUILD.txt file. The STM32 support is supplied via the ARM GCC compiler. The Teensy support uses Teensyduino.

This software is licenced under the GPL v2 and is primarily intended for amateur and educational use.

Portions of the ARM support code include the following copyright:

   Copyright (c) 2011 - 2013 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
