/*
 *   Copyright (C) 2016,2017,2018 by Jonathan Naylor G4KLX
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

#if !defined(NXDNDEFINES_H)
#define  NXDNDEFINES_H

const unsigned int NXDN_RADIO_SYMBOL_LENGTH = 10U;      // At 24 kHz sample rate

const unsigned int NXDN_FRAME_LENGTH_BITS    = 384U;
const unsigned int NXDN_FRAME_LENGTH_BYTES   = NXDN_FRAME_LENGTH_BITS / 8U;
const unsigned int NXDN_FRAME_LENGTH_SYMBOLS = NXDN_FRAME_LENGTH_BITS / 2U;
const unsigned int NXDN_FRAME_LENGTH_SAMPLES = NXDN_FRAME_LENGTH_SYMBOLS * NXDN_RADIO_SYMBOL_LENGTH;

const unsigned int NXDN_FSW_LENGTH_BITS    = 20U;
const unsigned int NXDN_FSW_LENGTH_SYMBOLS = NXDN_FSW_LENGTH_BITS / 2U;
const unsigned int NXDN_FSW_LENGTH_SAMPLES = NXDN_FSW_LENGTH_SYMBOLS * NXDN_RADIO_SYMBOL_LENGTH;

const uint8_t NXDN_FSW_BYTES[]      = {0xCDU, 0xF5U, 0x90U};
const uint8_t NXDN_FSW_BYTES_MASK[] = {0xFFU, 0xFFU, 0xF0U};
const uint8_t NXDN_FSW_BYTES_LENGTH = 3U;

const uint32_t NXDN_FSW_BITS      = 0x000CDF59U;
const uint32_t NXDN_FSW_BITS_MASK = 0x000FFFFFU;

// C     D      F     5      9
// 11 00 11 01  11 11 01 01  10 01
// -3 +1 -3 +3  -3 -3 +3 +3  -1 +3

const int8_t NXDN_FSW_SYMBOLS_VALUES[] = {-3, +1, -3, +3, -3, -3, +3, +3, -1, +3};

const uint16_t NXDN_FSW_SYMBOLS      = 0x014DU;
const uint16_t NXDN_FSW_SYMBOLS_MASK = 0x03FFU;

#endif

