/*
 *   Copyright (C) 2016,2017,2018,2020 by Jonathan Naylor G4KLX
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

#if !defined(M17DEFINES_H)
#define  M17DEFINES_H

const unsigned int M17_RADIO_SYMBOL_LENGTH = 5U;      // At 24 kHz sample rate

const unsigned int M17_FRAME_LENGTH_BITS    = 384U;
const unsigned int M17_FRAME_LENGTH_BYTES   = M17_FRAME_LENGTH_BITS / 8U;
const unsigned int M17_FRAME_LENGTH_SYMBOLS = M17_FRAME_LENGTH_BITS / 2U;
const unsigned int M17_FRAME_LENGTH_SAMPLES = M17_FRAME_LENGTH_SYMBOLS * M17_RADIO_SYMBOL_LENGTH;

const unsigned int M17_SYNC_LENGTH_BITS    = 16U;
const unsigned int M17_SYNC_LENGTH_SYMBOLS = M17_SYNC_LENGTH_BITS / 2U;
const unsigned int M17_SYNC_LENGTH_SAMPLES = M17_SYNC_LENGTH_SYMBOLS * M17_RADIO_SYMBOL_LENGTH;

const uint8_t M17_SYNC_BYTES[]      = {0xCDU, 0xF5U};	// XXX
const uint8_t M17_SYNC_BYTES_LENGTH = 2U;

const uint16_t M17_SYNC_BITS      = 0xCDF5U;			// XXX

// C     D      F      5					// XXX
// 11 00 11 01  11 11  01 01					// XXX
// -3 +1 -3 +3  -3 -3  +3 +3					// XXX

const int8_t M17_SYNC_SYMBOLS_VALUES[] = {-3, +1, -3, +3, -3, -3, +3, +3};	// XXX

const uint16_t M17_SYNC_SYMBOLS = 0x014DU;			// XXX

#endif

