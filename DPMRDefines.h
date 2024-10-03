/*
 *   Copyright (C) 2016,2017,2018,2024 by Jonathan Naylor G4KLX
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

#if !defined(DPMRDEFINES_H)
#define  DPMRDEFINES_H

const unsigned int DPMR_RADIO_SYMBOL_LENGTH = 10U;      // At 24 kHz sample rate

const unsigned int DPMR_HEADER_FRAME_LENGTH_BITS    = 312U;
const unsigned int DPMR_HEADER_FRAME_LENGTH_BYTES   = DPMR_HEADER_FRAME_LENGTH_BITS / 8U;
const unsigned int DPMR_HEADER_FRAME_LENGTH_SYMBOLS = DPMR_HEADER_FRAME_LENGTH_BITS / 2U;
const unsigned int DPMR_HEADER_FRAME_LENGTH_SAMPLES = DPMR_HEADER_FRAME_LENGTH_SYMBOLS * DPMR_RADIO_SYMBOL_LENGTH;

const unsigned int DPMR_DATA_FRAME_LENGTH_BITS    = 384U;
const unsigned int DPMR_DATA_FRAME_LENGTH_BYTES   = DPMR_DATA_FRAME_LENGTH_BITS / 8U;
const unsigned int DPMR_DATA_FRAME_LENGTH_SYMBOLS = DPMR_DATA_FRAME_LENGTH_BITS / 2U;
const unsigned int DPMR_DATA_FRAME_LENGTH_SAMPLES = DPMR_DATA_FRAME_LENGTH_SYMBOLS * DPMR_RADIO_SYMBOL_LENGTH;

const unsigned int DPMR_END_FRAME_LENGTH_BITS    = 96U;
const unsigned int DPMR_END_FRAME_LENGTH_BYTES   = DPMR_END_FRAME_LENGTH_BITS / 8U;
const unsigned int DPMR_END_FRAME_LENGTH_SYMBOLS = DPMR_END_FRAME_LENGTH_BITS / 2U;
const unsigned int DPMR_END_FRAME_LENGTH_SAMPLES = DPMR_END_FRAME_LENGTH_SYMBOLS * DPMR_RADIO_SYMBOL_LENGTH;

const unsigned int DPMR_FS1_LENGTH_BITS    = 48U;
const unsigned int DPMR_FS1_LENGTH_SYMBOLS = DPMR_FS1_LENGTH_BITS / 2U;
const unsigned int DPMR_FS1_LENGTH_SAMPLES = DPMR_FS1_LENGTH_SYMBOLS * DPMR_RADIO_SYMBOL_LENGTH;

const unsigned int DPMR_FS2_LENGTH_BITS    = 24U;
const unsigned int DPMR_FS2_LENGTH_SYMBOLS = DPMR_FS2_LENGTH_BITS / 2U;
const unsigned int DPMR_FS2_LENGTH_SAMPLES = DPMR_FS2_LENGTH_SYMBOLS * DPMR_RADIO_SYMBOL_LENGTH;

const unsigned int DPMR_FS3_LENGTH_BITS    = 24U;
const unsigned int DPMR_FS3_LENGTH_SYMBOLS = DPMR_FS3_LENGTH_BITS / 2U;
const unsigned int DPMR_FS3_LENGTH_SAMPLES = DPMR_FS3_LENGTH_SYMBOLS * DPMR_RADIO_SYMBOL_LENGTH;

const unsigned int DPMR_FS4_LENGTH_BITS    = 48U;
const unsigned int DPMR_FS4_LENGTH_SYMBOLS = DPMR_FS4_LENGTH_BITS / 2U;
const unsigned int DPMR_FS4_LENGTH_SAMPLES = DPMR_FS4_LENGTH_SYMBOLS * DPMR_RADIO_SYMBOL_LENGTH;

const uint8_t DPMR_FS1_BYTES[]      = {0x57U, 0xFFU, 0x5FU, 0x75U, 0xD5U, 0x77U};
const uint8_t DPMR_FS1_BYTES_LENGTH = 6U;

const uint8_t DPMR_FS2_BYTES[]      = {0x5FU, 0xF7U, 0x7DU};
const uint8_t DPMR_FS2_BYTES_LENGTH = 3U;

const uint8_t DPMR_FS3_BYTES[]      = {0x7DU, 0xDFU, 0xF5U};
const uint8_t DPMR_FS3_BYTES_LENGTH = 3U;

const uint8_t DPMR_FS4_BYTES[]      = {0xFDU, 0x55U, 0xF5U, 0xDFU, 0x7FU, 0xDDU};
const uint8_t DPMR_FS4_BYTES_LENGTH = 6U;

const uint64_t DPMR_FS1_BITS      = 0x000057FF5F75D577U;
const uint64_t DPMR_FS1_BITS_MASK = 0x0000FFFFFFFFFFFFU;

const uint64_t DPMR_FS2_BITS      = 0x00000000005FF77DU;
const uint64_t DPMR_FS2_BITS_MASK = 0x0000000000FFFFFFU;

const uint64_t DPMR_FS3_BITS      = 0x00000000007DDFF5U;
const uint64_t DPMR_FS3_BITS_MASK = 0x0000000000FFFFFFU;

const uint64_t DPMR_FS4_BITS      = 0x0000FD55F5DF7FDDU;
const uint64_t DPMR_FS4_BITS_MASK = 0x0000FFFFFFFFFFFFU;

// FS1
// 5     5      F     F      5     F      7     5      D     5      7     7
// 01 01 01 11  11 11 11 11  01 01 11 11  01 11 01 01  11 01 01 01  01 11 01 11
// +3 +3 +3 -3  -3 -3 -3 -3  +3 +3 -3 -3  +3 -3 +3 +3  -3 +3 +3 +3  +3 -3 +3 -3

const int8_t DPMR_FS1_SYMBOLS_VALUES[] = {+3, +3, +3, -3, -3, -3, -3, -3, +3, +3, -3, -3, +3, -3, +3, +3, -3, +3, +3, +3, +3, -3, +3, -3};

const uint32_t DPMR_FS1_SYMBOLS      = 0x00E0CB7AU;
const uint32_t DPMR_FS1_SYMBOLS_MASK = 0x00FFFFFFU;

// FS2
// 5     F      F     7      7     D
// 01 01 11 11  11 11 01 11  01 11 11 01
// +3 +3 -3 -3  -3 -3 +3 -3  +3 -3 -3 +3

const int8_t DPMR_FS2_SYMBOLS_VALUES[] = {+3, +3, -3, -3, -3, -3, +3, -3, +3, -3, -3, +3};

const uint32_t DPMR_FS2_SYMBOLS      = 0x00000C29U;
const uint32_t DPMR_FS2_SYMBOLS_MASK = 0x00000FFFU;

// FS3
// 7     D      D     F      F     5
// 01 11 11 01  11 01 11 11  11 11 01 01
// +3 -3 -3 +3  -3 +3 -3 -3  -3 -3 +3 +3

const int8_t DPMR_FS3_SYMBOLS_VALUES[] = {+3, -3, -3, +3, -3, +3, -3, -3, -3, -3, +3, +3};

const uint32_t DPMR_FS3_SYMBOLS      = 0x00000943U;
const uint32_t DPMR_FS3_SYMBOLS_MASK = 0x00000FFFU;

// FS4
// F     D      5     5      F     5      D     F      7     F      D     D
// 11 11 11 01  01 01 01 01  11 11 01 01  11 01 11 11  01 11 11 11  11 01 11 01
// -3 -3 -3 +3  +3 +3 +3 +3  -3 -3 +3 +3  -3 +3 -3 -3  +3 -3 -3 -3  -3 +3 -3 +3

const int8_t DPMR_FS4_SYMBOLS_VALUES[] = {-3, -3, -3, +3, +3, +3, +3, +3, -3, -3, +3, +3, -3, +3, -3, -3, +3, -3, -3, -3, -3, +3, -3, +3};

const uint32_t DPMR_FS4_SYMBOLS      = 0x001F3485U;
const uint32_t DPMR_FS4_SYMBOLS_MASK = 0x00FFFFFFU;

#endif

