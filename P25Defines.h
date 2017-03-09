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

#if !defined(P25DEFINES_H)
#define  P25DEFINES_H

const unsigned int P25_RADIO_SYMBOL_LENGTH = 5U;      // At 24 kHz sample rate

const unsigned int P25_HDR_FRAME_LENGTH_BYTES      = 99U;
const unsigned int P25_HDR_FRAME_LENGTH_BITS       = P25_HDR_FRAME_LENGTH_BYTES * 8U;
const unsigned int P25_HDR_FRAME_LENGTH_SYMBOLS    = P25_HDR_FRAME_LENGTH_BYTES * 4U;
const unsigned int P25_HDR_FRAME_LENGTH_SAMPLES    = P25_HDR_FRAME_LENGTH_SYMBOLS * P25_RADIO_SYMBOL_LENGTH;

const unsigned int P25_LDU_FRAME_LENGTH_BYTES      = 216U;
const unsigned int P25_LDU_FRAME_LENGTH_BITS       = P25_LDU_FRAME_LENGTH_BYTES * 8U;
const unsigned int P25_LDU_FRAME_LENGTH_SYMBOLS    = P25_LDU_FRAME_LENGTH_BYTES * 4U;
const unsigned int P25_LDU_FRAME_LENGTH_SAMPLES    = P25_LDU_FRAME_LENGTH_SYMBOLS * P25_RADIO_SYMBOL_LENGTH;

const unsigned int P25_TERMLC_FRAME_LENGTH_BYTES   = 54U;
const unsigned int P25_TERMLC_FRAME_LENGTH_BITS    = P25_TERMLC_FRAME_LENGTH_BYTES * 8U;
const unsigned int P25_TERMLC_FRAME_LENGTH_SYMBOLS = P25_TERMLC_FRAME_LENGTH_BYTES * 4U;
const unsigned int P25_TERMLC_FRAME_LENGTH_SAMPLES = P25_TERMLC_FRAME_LENGTH_SYMBOLS * P25_RADIO_SYMBOL_LENGTH;

const unsigned int P25_TERM_FRAME_LENGTH_BYTES     = 18U;
const unsigned int P25_TERM_FRAME_LENGTH_BITS      = P25_TERM_FRAME_LENGTH_BYTES * 8U;
const unsigned int P25_TERM_FRAME_LENGTH_SYMBOLS   = P25_TERM_FRAME_LENGTH_BYTES * 4U;
const unsigned int P25_TERM_FRAME_LENGTH_SAMPLES   = P25_TERM_FRAME_LENGTH_SYMBOLS * P25_RADIO_SYMBOL_LENGTH;

const unsigned int P25_SYNC_LENGTH_BYTES   = 6U;
const unsigned int P25_SYNC_LENGTH_BITS    = P25_SYNC_LENGTH_BYTES * 8U;
const unsigned int P25_SYNC_LENGTH_SYMBOLS = P25_SYNC_LENGTH_BYTES * 4U;
const unsigned int P25_SYNC_LENGTH_SAMPLES = P25_SYNC_LENGTH_SYMBOLS * P25_RADIO_SYMBOL_LENGTH;

const unsigned int P25_NID_LENGTH_BITS     = 64U;
const unsigned int P25_NID_LENGTH_SYMBOLS  = 32U;
const unsigned int P25_NID_LENGTH_SAMPLESS = P25_NID_LENGTH_SYMBOLS * P25_RADIO_SYMBOL_LENGTH;

const uint8_t P25_SYNC_BYTES[] = {0x55U, 0x75U, 0xF5U, 0xFFU, 0x77U, 0xFFU};
const uint8_t P25_SYNC_BYTES_LENGTH  = 6U;

const uint64_t P25_SYNC_BITS      = 0x00005575F5FF77FFU;
const uint64_t P25_SYNC_BITS_MASK = 0x0000FFFFFFFFFFFFU;

// 5     5      7     5      F     5      F     F      7     7      F     F
// 01 01 01 01  01 11 01 01  11 11 01 01  11 11 11 11  01 11 01 11  11 11 11 11
// +3 +3 +3 +3  +3 -3 +3 +3  -3 -3 +3 +3  -3 -3 -3 -3  +3 -3 +3 -3  -3 -3 -3 -3

const int8_t P25_SYNC_SYMBOLS_VALUES[] = {+3, +3, +3, +3, +3, -3, +3, +3, -3, -3, +3, +3, -3, -3, -3, -3, +3, -3, +3, -3, -3, -3, -3, -3};

const uint32_t P25_SYNC_SYMBOLS      = 0x00FB30A0U;
const uint32_t P25_SYNC_SYMBOLS_MASK = 0x00FFFFFFU;

#endif

