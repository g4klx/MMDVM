/*
 *   Copyright (C) 2015,2016,2017,2018,2020 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#if !defined(CONFIG_H)
#define  CONFIG_H

// Allow for the selection of which modes to compile into the firmware. This is particularly useful for processors
// which have limited code space and processing power.

// Enable D-Star support.
#define MODE_DSTAR

// Enable DMR support.
#define MODE_DMR

// Enable System Fusion support.
#define MODE_YSF

// Enable P25 phase 1 support.
#define MODE_P25

// Enable NXDN support, the boxcar filter sometimes improves the performance of NXDN receive on some systems.
#define MODE_NXDN
#define USE_NXDN_BOXCAR

// Enable M17 support.
#define MODE_M17

// Enable POCSAG support.
#define MODE_POCSAG

// Enable FM support.
#define MODE_FM

// Enable AX.25 support, this is only enabled if FM is also enabled.
#define MODE_AX25

// Allow for the use of high quality external clock oscillators
// The number is the frequency of the oscillator in Hertz.
//
// The frequency of the TCXO must be an integer multiple of 48000.
// Frequencies such as 12.0 Mhz (48000 * 250) and 14.4 Mhz (48000 * 300) are suitable.
// Frequencies such as 10.0 Mhz (48000 * 208.333) or 20 Mhz (48000 * 416.666) are not suitable.
//
// For 12 MHz
#define EXTERNAL_OSC 12000000
// For 12.288 MHz
// #define EXTERNAL_OSC 12288000
// For 14.4 MHz
// #define EXTERNAL_OSC 14400000
// For 19.2 MHz
// #define EXTERNAL_OSC 19200000

// Select a baud rate for host communication. The faster speeds are needed for external FM to work.
// #define SERIAL_SPEED 115200 // Suitable for most older boards (Arduino Due, etc). External FM will NOT work with this!
// #define SERIAL_SPEED 230400 // Only works on newer boards like fast M4, M7, Teensy 3.x. External FM might work with this
#define SERIAL_SPEED 460800	// Only works on newer boards like fast M4, M7, Teensy 3.x. External FM should work with this
//#define SERIAL_SPEED 500000  // Used with newer boards and Armbian on AllWinner SOCs (H2, H3) that do not support 460800

// Use pins to output the current mode via LEDs
#define MODE_LEDS

// For the original Arduino Due pin layout
// #define ARDUINO_DUE_PAPA

// For the ZUM V1.0 and V1.0.1 boards pin layout
// #define ARDUINO_DUE_ZUM_V10

// For the SP8NTH board
// #define ARDUINO_DUE_NTH

// For ST Nucleo-64 STM32F446RE board
// #define STM32F4_NUCLEO_MORPHO_HEADER
// #define STM32F4_NUCLEO_ARDUINO_HEADER

// Use separate mode pins to switch external channel/filters/bandwidth for example
// #define MODE_PINS

// For the VK6MST Pi3 Shield communicating over i2c. i2c address & speed defined in i2cTeensy.cpp
// #define VK6MST_TEENSY_PI3_SHIELD_I2C

// Pass RSSI information to the host
#define SEND_RSSI_DATA

// Use the modem as a serial repeater for Nextion displays
#define SERIAL_REPEATER

// Set the baud rate of the modem serial repeater for Nextion displays
#define SERIAL_REPEATER_BAUD_RATE 9600

// Use the modem as an I2C repeater for OLED displays
// #define I2C_REPEATER

// To reduce CPU load, you can remove the DC blocker by commenting out the next line
#define USE_DCBLOCKER

// Constant Service LED once repeater is running 
// Do not use if employing an external hardware watchdog 
// #define CONSTANT_SRV_LED

// Use the YSF and P25 LEDs for NXDN
// #define USE_ALTERNATE_NXDN_LEDS

// Use the D-Star and P25 LEDs for M17
#define USE_ALTERNATE_M17_LEDS

// Use the D-Star and DMR LEDs for POCSAG
#define USE_ALTERNATE_POCSAG_LEDS

// Use the D-Star and YSF LEDs for FM
#define USE_ALTERNATE_FM_LEDS

#endif

