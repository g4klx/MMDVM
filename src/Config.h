/*
 *   Copyright (C) 2015,2016,2017,2018,2020,2024 by Jonathan Naylor G4KLX
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

// Select a baud rate for host communication. The faster speeds are needed for external FM to work.
// #define SERIAL_SPEED 115200 // Suitable for most older boards. External FM will NOT work with this!
// #define SERIAL_SPEED 230400 // Only works on newer boards like fast M4, M7. External FM might work with this
#define SERIAL_SPEED 460800	// Only works on newer boards like fast M4, M7. External FM will work with this
//#define SERIAL_SPEED 500000  // Used with newer boards and Armbian on AllWinner SOCs (H2, H3) that do not support 460800

// Use pins to output the current mode via LEDs
#define MODE_LEDS

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

// Choose the type of modem board in use, only one.
// The KI6ZUM v0.9 Pi hat, STM32F446 CPU
#define ZUM_V09_PI
// The KI6ZUM v1.0 Pi hat, STM32F722 CPU
// #define ZUM_V10_PI
// The Repeater Builder V3 board, STM32F446 CPU
// #define RB_V3_PI
// The Repeater Builder V5 board, STM32F722 CPU
// #define RB_V5_PI

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

