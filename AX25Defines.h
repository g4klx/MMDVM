/*
 *   Copyright (C) 2020 by Jonathan Naylor G4KLX
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

#if !defined(AX25DEFINES_H)
#define  AX25DEFINES_H

const uint8_t AX25_RADIO_SYMBOL_LENGTH = 20U;      // At 24 kHz sample rate

const uint8_t AX25_FRAME_START = 0x7EU;
const uint8_t AX25_FRAME_END   = 0x7EU;
const uint8_t AX25_FRAME_ABORT = 0xFEU;

const uint8_t AX25_MAX_ONES    = 5U;

const uint16_t AX25_MIN_FRAME_LENGTH = 17U;        // Callsign (7) + Callsign (7) + Control (1) + Checksum (2)

const uint16_t AX25_MAX_FRAME_LENGTH = 330U;       // Callsign (7) + Callsign (7) + 8 Digipeaters (56) +
                                                   // Control (1) + PID (1) + Data (256) + Checksum (2)

#endif

