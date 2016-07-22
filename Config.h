/*
 *   Copyright (C) 2015,2016 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#if !defined(CONFIG_H)
#define  CONFIG_H

// Allow for the use of high quality external clock oscillators
// The number is the frequency of the oscillator in Hertz.
// For 12 MHz
// #define EXTERNAL_OSC 12000000
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

// For the SP8NTH board
// #define  ARDUINO_DUE_NTH

// To use wider C4FSK filters for DMR and System Fusion on transmit
// #define WIDE_C4FSK_FILTERS_TX
// To use wider C4FSK filters for DMR and System Fusion on receive
// #define WIDE_C4FSK_FILTERS_RX

#endif

