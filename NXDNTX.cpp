/*
 *   Copyright (C) 2009-2018 by Jonathan Naylor G4KLX
 *   Copyright (C) 2017 by Andy Uribe CA6JAU
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

#include "Config.h"
#include "Globals.h"
#include "NXDNTX.h"

#include "NXDNDefines.h"

// Generated using rcosdesign(0.2, 8, 20, 'sqrt') in MATLAB
static q15_t RRC_0_2_FILTER[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 201, 174, 140, 99, 52, 0, -55, -112, -170, -226, -278, -325, -365, -397, -417, -427, -424, -407, -377, -333, -277, -208,
		  -128, -40, 56, 156, 258, 358, 455, 544, 622, 687, 736, 766, 775, 762, 725, 664, 579, 471, 342, 193, 27, -151, -338, -528,
		  -718, -901, -1072, -1225, -1354, -1454, -1520, -1547, -1530, -1466, -1353, -1189, -972, -704, -385, -18, 394, 846, 1333,
		  1850, 2388, 2940, 3498, 4053, 4598, 5122, 5619, 6079, 6494, 6859, 7166, 7410, 7588, 7696, 7732, 7696, 7588, 7410, 7166,
		  6859, 6494, 6079, 5619, 5122, 4598, 4053, 3498, 2940, 2388, 1850, 1333, 846, 394, -18, -385, -704, -972, -1189, -1353,
		  -1466, -1530, -1547, -1520, -1454, -1354, -1225, -1072, -901, -718, -528, -338, -151, 27, 193, 342, 471, 579, 664, 725,
		  762, 775, 766, 736, 687, 622, 544, 455, 358, 258, 156, 56, -40, -128, -208, -277, -333, -377, -407, -424, -427, -417, -397,
		  -365, -325, -278, -226, -170, -112, -55, 0, 52, 99, 140, 174, 201}; // numTaps = 180, L = 20
const uint16_t RRC_0_2_FILTER_PHASE_LEN = 9U; // phaseLength = numTaps/L

static q15_t NXDN_SINC_FILTER[] = {572, -1003, -253, 254, 740, 1290, 1902, 2527, 3090, 3517, 3747, 3747, 3517, 3090, 2527, 1902,
                                   1290, 740, 254, -253, -1003, 572};
const uint16_t NXDN_SINC_FILTER_LEN = 22U;

const q15_t NXDN_LEVELA =  735;
const q15_t NXDN_LEVELB =  245;
const q15_t NXDN_LEVELC = -245;
const q15_t NXDN_LEVELD = -735;

const uint8_t NXDN_PREAMBLE[] = {0x57U, 0x75U, 0xFDU};
const uint8_t NXDN_SYNC = 0x5FU;

CNXDNTX::CNXDNTX() :
m_buffer(4000U),
m_modFilter(),
m_sincFilter(),
m_modState(),
m_sincState(),
m_poBuffer(),
m_poLen(0U),
m_poPtr(0U),
m_txDelay(240U)      // 200ms
{
  ::memset(m_modState, 0x00U, 16U * sizeof(q15_t));
  ::memset(m_sincState,  0x00U, 70U * sizeof(q15_t));

  m_modFilter.L           = NXDN_RADIO_SYMBOL_LENGTH;
  m_modFilter.phaseLength = RRC_0_2_FILTER_PHASE_LEN;
  m_modFilter.pCoeffs     = RRC_0_2_FILTER;
  m_modFilter.pState      = m_modState;

  m_sincFilter.numTaps = NXDN_SINC_FILTER_LEN;
  m_sincFilter.pState  = m_sincState;
  m_sincFilter.pCoeffs = NXDN_SINC_FILTER;
}

void CNXDNTX::process()
{
  if (m_buffer.getData() == 0U && m_poLen == 0U)
    return;

  if (m_poLen == 0U) {
    if (!m_tx) {
      for (uint16_t i = 0U; i < m_txDelay; i++)
        m_poBuffer[m_poLen++] = NXDN_SYNC;
      m_poBuffer[m_poLen++] = NXDN_PREAMBLE[0U];
      m_poBuffer[m_poLen++] = NXDN_PREAMBLE[1U];
      m_poBuffer[m_poLen++] = NXDN_PREAMBLE[2U];
    } else {
      for (uint8_t i = 0U; i < NXDN_FRAME_LENGTH_BYTES; i++) {
        uint8_t c = m_buffer.get();
        m_poBuffer[m_poLen++] = c;
      }
    }

    m_poPtr = 0U;
  }

  if (m_poLen > 0U) {
    uint16_t space = io.getSpace();
    
    while (space > (4U * NXDN_RADIO_SYMBOL_LENGTH)) {
      uint8_t c = m_poBuffer[m_poPtr++];
      writeByte(c);

      space -= 4U * NXDN_RADIO_SYMBOL_LENGTH;
      
      if (m_poPtr >= m_poLen) {
        m_poPtr = 0U;
        m_poLen = 0U;
        return;
      }
    }
  }
}

uint8_t CNXDNTX::writeData(const uint8_t* data, uint8_t length)
{
  if (length != (NXDN_FRAME_LENGTH_BYTES + 1U))
    return 4U;

  uint16_t space = m_buffer.getSpace();
  if (space < NXDN_FRAME_LENGTH_BYTES)
    return 5U;

  for (uint8_t i = 0U; i < NXDN_FRAME_LENGTH_BYTES; i++)
    m_buffer.put(data[i + 1U]);

  return 0U;
}

void CNXDNTX::writeByte(uint8_t c)
{
  q15_t inBuffer[4U];
  q15_t intBuffer[NXDN_RADIO_SYMBOL_LENGTH * 4U];
  q15_t outBuffer[NXDN_RADIO_SYMBOL_LENGTH * 4U];

  const uint8_t MASK = 0xC0U;

  for (uint8_t i = 0U; i < 4U; i++, c <<= 2) {
    switch (c & MASK) {
      case 0xC0U:
        inBuffer[i] = NXDN_LEVELA;
        break;
      case 0x80U:
        inBuffer[i] = NXDN_LEVELB;
        break;
      case 0x00U:
        inBuffer[i] = NXDN_LEVELC;
        break;
      default:
        inBuffer[i] = NXDN_LEVELD;
        break;
    }
  }

  ::arm_fir_interpolate_q15(&m_modFilter, inBuffer, intBuffer, 4U);

  ::arm_fir_fast_q15(&m_sincFilter, intBuffer, outBuffer, NXDN_RADIO_SYMBOL_LENGTH * 4U);

  io.write(STATE_NXDN, outBuffer, NXDN_RADIO_SYMBOL_LENGTH * 4U);
}

void CNXDNTX::setTXDelay(uint8_t delay)
{
  m_txDelay = 300U + uint16_t(delay) * 6U;        // 500ms + tx delay

  if (m_txDelay > 1200U)
    m_txDelay = 1200U;
}

uint8_t CNXDNTX::getSpace() const
{
  return m_buffer.getSpace() / NXDN_FRAME_LENGTH_BYTES;
}

