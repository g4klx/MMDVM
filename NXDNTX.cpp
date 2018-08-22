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

// NXDN RRC filter + SINC filter
static q15_t RRC_0_2_FILTER[] = {51, 71, 92, 112, 129, 141, 144, 138, 120, 87, 39, -26, -107, -204, -314, -437, -567, -702, -835,
                               -932,-1066, -1176, -1257, -1304, -1314, -1284, -1213, -1099, -944, -749, -519, -257, 30, 335, 651,
                               968, 1277, 1569, 1832, 2058, 2237, 2360, 2420, 2412, 2329, 2170, 1935, 1623, 1239, 790, 283, -271,
                               -858, -1466, -2076, -2671, -3233, -3742, -4178, -4521, -4754, -4857, -4815, -4614, -4242, -3690,
                               -2952, -2025, -912, 385, 1855, 3488, 5266, 7172, 9183, 11274, 13418, 15585, 17746, 19869, 21923,
                               23876, 25700, 27364, 28844, 30116, 31159, 31956, 32495, 32767, 32767, 32495, 31956, 31159, 30116,
                               28844, 27364, 25700, 23876, 21923, 19869, 17746, 15585, 13418, 11274, 9183, 7172, 5266, 3488, 1855,
                               385, -912, -2025, -2952, -3690, -4242, -4614, -4815, -4857, -4754, -4521, -4178, -3742, -3233, -2671,
                               -2076, -1466, -858, -271, 283, 790, 1239, 1623, 1935, 2170, 2329, 2412, 2420, 2360, 2237, 2058, 1832,
                               1569, 1277, 968, 651, 335, 30, -257, -519, -749, -944, -1099, -1213, -1284, -1314, -1304, -1257, 
                               -1176, -1066, -932, -835, -702, -567, -437, -314, -204, -107, -26, 39, 87, 120, 138, 144, 141, 129,
                               112, 92, 71, 51}; // numTaps = 180, L = 20
const uint16_t RRC_0_2_FILTER_PHASE_LEN = 9U; // phaseLength = numTaps/L

const q15_t NXDN_LEVELA =  735;
const q15_t NXDN_LEVELB =  245;
const q15_t NXDN_LEVELC = -245;
const q15_t NXDN_LEVELD = -735;

const uint8_t NXDN_PREAMBLE[] = {0x57U, 0x75U, 0xFDU};
const uint8_t NXDN_SYNC = 0x5FU;

CNXDNTX::CNXDNTX() :
m_buffer(TX_BUFFER_LEN),
m_modFilter(),
m_modState(),
m_poBuffer(),
m_poLen(0U),
m_poPtr(0U),
m_txDelay(240U)      // 200ms
{
  ::memset(m_modState, 0x00U, 16U * sizeof(q15_t));

  m_modFilter.L           = NXDN_RADIO_SYMBOL_LENGTH;
  m_modFilter.phaseLength = RRC_0_2_FILTER_PHASE_LEN;
  m_modFilter.pCoeffs     = RRC_0_2_FILTER;
  m_modFilter.pState      = m_modState;
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

  ::arm_fir_interpolate_q15(&m_modFilter, inBuffer, outBuffer, 4U);

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

