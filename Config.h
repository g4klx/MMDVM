/*
 *   Copyright (C) 2015,2016 by Jonathan Naylor G4KLX
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

// Allow for the use of high quality external clock oscillators
// The number is the frequency of the oscillator in Hertz.
// For 12 MHz
#define EXTERNAL_OSC 12000000
// For 14.4 MHz
// #define EXTERNAL_OSC 14400000
// For 19.2 MHz
// #define EXTERNAL_OSC 19200000

// Allow the use of the COS line to lockout the modem
// #define USE_COS_AS_LOCKOUT

// Use pins to output the current mode
// #define ARDUINO_MODE_PINS

// For the original Arduino Due pin layout
// #define  ARDUINO_DUE_PAPA

// For the ZUM Board V1.0 pin layout
#define  ARDUINO_DUE_ZUM_V10

// For the ZUM Board V1.2 pin layout
// #define  ARDUINO_DUE_ZUM_V12

// For the SP8NTH board
// #define  ARDUINO_DUE_NTH

// To use wider C4FSK filters for DMR, System Fusion and P25 on transmit
// #define WIDE_C4FSK_FILTERS_TX
// To use wider C4FSK filters for DMR, System Fusion and P25 on receive
// #define WIDE_C4FSK_FILTERS_RX

// Pass RSSI information to the host
// #define SEND_RSSI_DATA

// Use the modem as a serial repeater for Nextion displays
#define SERIAL_REPEATER

#endif

