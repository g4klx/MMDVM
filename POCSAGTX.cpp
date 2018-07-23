/*
 *   Copyright (C) 2009-2018 by Jonathan Naylor G4KLX
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
#include "POCSAGTX.h"

const uint16_t POCSAG_FRAME_LENGTH_BYTES = 17U * sizeof(uint32_t);

const uint16_t POCSAG_PREAMBLE_LENGTH_BYTES = 18U * sizeof(uint32_t);

const uint16_t POCSAG_RADIO_SYMBOL_LENGTH = 20U;

const q15_t POCSAG_LEVEL1[] = { 1700,  1700,  1700,  1700,  1700,  1700,  1700,  1700,  1700,  1700,  1700,  1700,  1700,  1700,  1700,  1700,  1700,  1700,  1700,  1700};
const q15_t POCSAG_LEVEL0[] = {-1700, -1700, -1700, -1700, -1700, -1700, -1700, -1700, -1700, -1700, -1700, -1700, -1700, -1700, -1700, -1700, -1700, -1700, -1700, -1700};

static q15_t SHAPING_FILTER[] = {5461, 5461, 5461, 5461, 5461, 5461};
const uint16_t SHAPING_FILTER_LEN = 6U;

const uint8_t POCSAG_SYNC = 0xAAU;

CPOCSAGTX::CPOCSAGTX() :
m_buffer(4000U),
m_modFilter(),
m_modState(),
m_poBuffer(),
m_poLen(0U),
m_poPtr(0U),
m_txDelay(POCSAG_PREAMBLE_LENGTH_BYTES)
{
  ::memset(m_modState, 0x00U, 170U * sizeof(q15_t));

  m_modFilter.numTaps = SHAPING_FILTER_LEN;
  m_modFilter.pState  = m_modState;
  m_modFilter.pCoeffs = SHAPING_FILTER;
}

void CPOCSAGTX::process()
{
  if (m_buffer.getData() == 0U && m_poLen == 0U)
    return;

  if (m_poLen == 0U) {
    if (!m_tx) {
      for (uint16_t i = 0U; i < m_txDelay; i++)
        m_poBuffer[m_poLen++] = POCSAG_SYNC;
    } else {
      for (uint8_t i = 0U; i < POCSAG_FRAME_LENGTH_BYTES; i++) {
        uint8_t c = m_buffer.get();
        m_poBuffer[m_poLen++] = c;
      }
    }

    m_poPtr = 0U;
  }

  if (m_poLen > 0U) {
    uint16_t space = io.getSpace();
    
    while (space > (8U * POCSAG_RADIO_SYMBOL_LENGTH)) {
      uint8_t c = m_poBuffer[m_poPtr++];
      writeByte(c);

      space -= 8U * POCSAG_RADIO_SYMBOL_LENGTH;
      
      if (m_poPtr >= m_poLen) {
        m_poPtr = 0U;
        m_poLen = 0U;
        return;
      }
    }
  }
}

bool CPOCSAGTX::busy()
{
  if (m_poLen > 0U || m_buffer.getData() > 0U)
    return true;
  else
    return false;
}

uint8_t CPOCSAGTX::writeData(const uint8_t* data, uint8_t length)
{
  if (length != POCSAG_FRAME_LENGTH_BYTES)
    return 4U;

  uint16_t space = m_buffer.getSpace();
  if (space < POCSAG_FRAME_LENGTH_BYTES)
    return 5U;

  for (uint8_t i = 0U; i < POCSAG_FRAME_LENGTH_BYTES; i++)
    m_buffer.put(data[i]);

  return 0U;
}

void CPOCSAGTX::writeByte(uint8_t c)
{
  q15_t inBuffer[POCSAG_RADIO_SYMBOL_LENGTH * 8U];
  q15_t outBuffer[POCSAG_RADIO_SYMBOL_LENGTH * 8U];

  const uint8_t MASK = 0x80U;

  uint8_t n = 0U;
  for (uint8_t i = 0U; i < 8U; i++, c <<= 1, n += POCSAG_RADIO_SYMBOL_LENGTH) {
    switch (c & MASK) {
      case 0x80U:
        ::memcpy(inBuffer + n, POCSAG_LEVEL1, POCSAG_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
        break;
      default:
        ::memcpy(inBuffer + n, POCSAG_LEVEL0, POCSAG_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
        break;
    }
  }

  ::arm_fir_fast_q15(&m_modFilter, inBuffer, outBuffer, POCSAG_RADIO_SYMBOL_LENGTH * 8U);

  io.write(STATE_POCSAG, outBuffer, POCSAG_RADIO_SYMBOL_LENGTH * 8U);
}

void CPOCSAGTX::setTXDelay(uint8_t delay)
{
  m_txDelay = POCSAG_PREAMBLE_LENGTH_BYTES + (delay * 3U) / 2U;

  if (m_txDelay > 150U)
    m_txDelay = 150U;
}

uint8_t CPOCSAGTX::getSpace() const
{
  return m_buffer.getSpace() / POCSAG_FRAME_LENGTH_BYTES;
}

