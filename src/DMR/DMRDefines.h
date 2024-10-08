/*
 *   Copyright (C) 2009-2016 by Jonathan Naylor G4KLX
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

#if !defined(DMRDEFINES_H)
#define  DMRDEFINES_H

const unsigned int DMR_RADIO_SYMBOL_LENGTH = 5U;      // At 24 kHz sample rate

const unsigned int DMR_FRAME_LENGTH_BYTES   = 33U;
const unsigned int DMR_FRAME_LENGTH_BITS    = DMR_FRAME_LENGTH_BYTES * 8U;
const unsigned int DMR_FRAME_LENGTH_SYMBOLS = DMR_FRAME_LENGTH_BYTES * 4U;
const unsigned int DMR_FRAME_LENGTH_SAMPLES = DMR_FRAME_LENGTH_SYMBOLS * DMR_RADIO_SYMBOL_LENGTH;

const unsigned int DMR_SYNC_LENGTH_BYTES   = 6U;
const unsigned int DMR_SYNC_LENGTH_BITS    = DMR_SYNC_LENGTH_BYTES * 8U;
const unsigned int DMR_SYNC_LENGTH_SYMBOLS = DMR_SYNC_LENGTH_BYTES * 4U;
const unsigned int DMR_SYNC_LENGTH_SAMPLES = DMR_SYNC_LENGTH_SYMBOLS * DMR_RADIO_SYMBOL_LENGTH;

const unsigned int DMR_EMB_LENGTH_BITS    = 16U;
const unsigned int DMR_EMB_LENGTH_SYMBOLS = 8U;
const unsigned int DMR_EMB_LENGTH_SAMPLES = DMR_EMB_LENGTH_SYMBOLS * DMR_RADIO_SYMBOL_LENGTH;

const unsigned int DMR_EMBSIG_LENGTH_BITS    = 32U;
const unsigned int DMR_EMBSIG_LENGTH_SYMBOLS = 16U;
const unsigned int DMR_EMBSIG_LENGTH_SAMPLES = DMR_EMBSIG_LENGTH_SYMBOLS * DMR_RADIO_SYMBOL_LENGTH;

const unsigned int DMR_SLOT_TYPE_LENGTH_BITS    = 20U;
const unsigned int DMR_SLOT_TYPE_LENGTH_SYMBOLS = 10U;
const unsigned int DMR_SLOT_TYPE_LENGTH_SAMPLES = DMR_SLOT_TYPE_LENGTH_SYMBOLS * DMR_RADIO_SYMBOL_LENGTH;

const unsigned int DMR_INFO_LENGTH_BITS    = 196U;
const unsigned int DMR_INFO_LENGTH_SYMBOLS = 98U;
const unsigned int DMR_INFO_LENGTH_SAMPLES = DMR_INFO_LENGTH_SYMBOLS * DMR_RADIO_SYMBOL_LENGTH;

const unsigned int DMR_AUDIO_LENGTH_BITS    = 216U;
const unsigned int DMR_AUDIO_LENGTH_SYMBOLS = 108U;
const unsigned int DMR_AUDIO_LENGTH_SAMPLES = DMR_AUDIO_LENGTH_SYMBOLS * DMR_RADIO_SYMBOL_LENGTH;

const unsigned int DMR_CACH_LENGTH_BYTES   = 3U;
const unsigned int DMR_CACH_LENGTH_BITS    = DMR_CACH_LENGTH_BYTES * 8U;
const unsigned int DMR_CACH_LENGTH_SYMBOLS = DMR_CACH_LENGTH_BYTES * 4U;
const unsigned int DMR_CACH_LENGTH_SAMPLES = DMR_CACH_LENGTH_SYMBOLS * DMR_RADIO_SYMBOL_LENGTH;

const uint8_t  DMR_SYNC_BYTES_LENGTH     = 7U;
const uint8_t  DMR_MS_DATA_SYNC_BYTES[]  = {0x0DU, 0x5DU, 0x7FU, 0x77U, 0xFDU, 0x75U, 0x70U};
const uint8_t  DMR_MS_VOICE_SYNC_BYTES[] = {0x07U, 0xF7U, 0xD5U, 0xDDU, 0x57U, 0xDFU, 0xD0U};
const uint8_t  DMR_BS_DATA_SYNC_BYTES[]  = {0x0DU, 0xFFU, 0x57U, 0xD7U, 0x5DU, 0xF5U, 0xD0U};
const uint8_t  DMR_BS_VOICE_SYNC_BYTES[] = {0x07U, 0x55U, 0xFDU, 0x7DU, 0xF7U, 0x5FU, 0x70U};
const uint8_t  DMR_SYNC_BYTES_MASK[]     = {0x0FU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xF0U};

const uint64_t DMR_MS_DATA_SYNC_BITS  = 0x0000D5D7F77FD757U;
const uint64_t DMR_MS_VOICE_SYNC_BITS = 0x00007F7D5DD57DFDU;
const uint64_t DMR_BS_DATA_SYNC_BITS  = 0x0000DFF57D75DF5DU;
const uint64_t DMR_BS_VOICE_SYNC_BITS = 0x0000755FD7DF75F7U;
const uint64_t DMR_SYNC_BITS_MASK     = 0x0000FFFFFFFFFFFFU;

const uint32_t DMR_MS_DATA_SYNC_SYMBOLS  = 0x0076286EU;
const uint32_t DMR_MS_VOICE_SYNC_SYMBOLS = 0x0089D791U;
const uint32_t DMR_BS_DATA_SYNC_SYMBOLS  = 0x00439B4DU;
const uint32_t DMR_BS_VOICE_SYNC_SYMBOLS = 0x00BC64B2U;
const uint32_t DMR_SYNC_SYMBOLS_MASK     = 0x00FFFFFFU;

// D     5      D     7      F     7      7     F      D     7      5     7
// 11 01 01 01  11 01 01 11  11 11 01 11  01 11 11 11  11 01 01 11  01 01 01 11
// -3 +3 +3 +3  -3 +3 +3 -3  -3 -3 +3 -3  +3 -3 -3 -3  -3 +3 +3 -3  +3 +3 +3 -3

const int8_t DMR_MS_DATA_SYNC_SYMBOLS_VALUES[] = {-3, +3, +3, +3, -3, +3, +3, -3, -3, -3, +3, -3, +3, -3, -3, -3, -3, +3, +3, -3, +3, +3, +3, -3};

// 7     F      7     D      5     D      D     5      7     D      F     D
// 01 11 11 11  01 11 11 01  01 01 11 01  11 01 01 01  01 11 11 01  11 11 11 01
// +3 -3 -3 -3  +3 -3 -3 +3  +3 +3 -3 +3  -3 +3 +3 +3  +3 -3 -3 +3  -3 -3 -3 +3

const int8_t DMR_MS_VOICE_SYNC_SYMBOLS_VALUES[] = {+3, -3, -3, -3, +3, -3, -3, +3, +3, +3, -3, +3, -3, +3, +3, +3, +3, -3, -3, +3, -3, -3, -3, +3};

const uint8_t DT_VOICE_PI_HEADER    = 0U;
const uint8_t DT_VOICE_LC_HEADER    = 1U;
const uint8_t DT_TERMINATOR_WITH_LC = 2U;
const uint8_t DT_CSBK               = 3U;
const uint8_t DT_DATA_HEADER        = 6U;
const uint8_t DT_RATE_12_DATA       = 7U;
const uint8_t DT_RATE_34_DATA       = 8U;
const uint8_t DT_IDLE               = 9U;
const uint8_t DT_RATE_1_DATA        = 10U;

#endif

