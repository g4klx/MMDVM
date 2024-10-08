/*
 *   Copyright (C) 2016,2017 by Jonathan Naylor G4KLX
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

#if !defined(YSFDEFINES_H)
#define  YSFDEFINES_H

const unsigned int YSF_RADIO_SYMBOL_LENGTH = 5U;      // At 24 kHz sample rate

const unsigned int YSF_FRAME_LENGTH_BYTES   = 120U;
const unsigned int YSF_FRAME_LENGTH_BITS    = YSF_FRAME_LENGTH_BYTES * 8U;
const unsigned int YSF_FRAME_LENGTH_SYMBOLS = YSF_FRAME_LENGTH_BYTES * 4U;
const unsigned int YSF_FRAME_LENGTH_SAMPLES = YSF_FRAME_LENGTH_SYMBOLS * YSF_RADIO_SYMBOL_LENGTH;

const unsigned int YSF_SYNC_LENGTH_BYTES   = 5U;
const unsigned int YSF_SYNC_LENGTH_BITS    = YSF_SYNC_LENGTH_BYTES * 8U;
const unsigned int YSF_SYNC_LENGTH_SYMBOLS = YSF_SYNC_LENGTH_BYTES * 4U;
const unsigned int YSF_SYNC_LENGTH_SAMPLES = YSF_SYNC_LENGTH_SYMBOLS * YSF_RADIO_SYMBOL_LENGTH;

const unsigned int YSF_FICH_LENGTH_BITS    = 200U;
const unsigned int YSF_FICH_LENGTH_SYMBOLS = 100U;
const unsigned int YSF_FICH_LENGTH_SAMPLES = YSF_FICH_LENGTH_SYMBOLS * YSF_RADIO_SYMBOL_LENGTH;

const uint8_t YSF_SYNC_BYTES[] = {0xD4U, 0x71U, 0xC9U, 0x63U, 0x4DU};
const uint8_t YSF_SYNC_BYTES_LENGTH  = 5U;

const uint64_t YSF_SYNC_BITS      = 0x000000D471C9634DU;
const uint64_t YSF_SYNC_BITS_MASK = 0x000000FFFFFFFFFFU;

// D     4      7     1      C     9      6     3      4     D
// 11 01 01 00  01 11 00 01  11 00 10 01  01 10 00 11  01 00 11 01
// -3 +3 +3 +1  +3 -3 +1 +3  -3 +1 -1 +3  +3 -1 +3 -3  +3 +1 -3 +3

const int8_t YSF_SYNC_SYMBOLS_VALUES[] = {-3, +3, +3, +1, +3, -3, +1, +3, -3, +1, -1, +3, +3, -1, +3, -3, +3, +1, -3, +3};

const uint32_t YSF_SYNC_SYMBOLS      = 0x0007B5ADU;
const uint32_t YSF_SYNC_SYMBOLS_MASK = 0x000FFFFFU;

#endif

