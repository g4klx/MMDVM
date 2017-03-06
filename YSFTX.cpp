/*
 *   Copyright (C) 2009-2017 by Jonathan Naylor G4KLX
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

// #define WANT_DEBUG

#include "Config.h"
#include "Globals.h"
#include "YSFTX.h"

#include "YSFDefines.h"

#if defined(WIDE_C4FSK_FILTERS_TX)
// Generated using rcosdesign(0.2, 4, 5, 'sqrt') in MATLAB
static q15_t YSF_C4FSK_FILTER[] = {688, -680, -2158, -3060, -2724, -775, 2684, 7041, 11310, 14425, 15565, 14425,
                                   11310, 7041, 2684, -775, -2724, -3060, -2158, -680, 688, 0};
const uint16_t YSF_C4FSK_FILTER_LEN = 22U;
#else
// Generated using rcosdesign(0.2, 8, 5, 'sqrt') in MATLAB
static q15_t YSF_C4FSK_FILTER[] = {401, 104, -340, -731, -847, -553, 112, 909, 1472, 1450, 683, -675, -2144, -3040, -2706, -770, 2667, 6995,
                                   11237, 14331, 15464, 14331, 11237, 6995, 2667, -770, -2706, -3040, -2144, -675, 683, 1450, 1472, 909, 112,
                                   -553, -847, -731, -340, 104, 401, 0};
const uint16_t YSF_C4FSK_FILTER_LEN = 42U;
#endif

const q15_t YSF_LEVELA[] = { 809,  809,  809,  809,  809};
const q15_t YSF_LEVELB[] = { 269,  269,  269,  269,  269};
const q15_t YSF_LEVELC[] = {-269, -269, -269, -269, -269};
const q15_t YSF_LEVELD[] = {-809, -809, -809, -809, -809};

const uint8_t YSF_START_SYNC = 0x77U;
const uint8_t YSF_END_SYNC   = 0xFFU;

CYSFTX::CYSFTX() :
m_buffer(1500U),
m_modFilter(),
m_modState(),
m_poBuffer(),
m_poLen(0U),
m_poPtr(0U),
m_txDelay(240U),      // 200ms
m_count(0U)
{
  ::memset(m_modState, 0x00U, 70U * sizeof(q15_t));

  m_modFilter.numTaps = YSF_C4FSK_FILTER_LEN;
  m_modFilter.pState  = m_modState;
  m_modFilter.pCoeffs = YSF_C4FSK_FILTER;
}

void CYSFTX::process()
{
  if (m_buffer.getData() == 0U && m_poLen == 0U)
    return;

  if (m_poLen == 0U) {
    if (!m_tx) {
      m_count = 0U;

      for (uint16_t i = 0U; i < m_txDelay; i++)
        m_poBuffer[m_poLen++] = YSF_START_SYNC;
    } else {
      for (uint8_t i = 0U; i < YSF_FRAME_LENGTH_BYTES; i++) {
        uint8_t c = m_buffer.get();
        m_poBuffer[m_poLen++] = c;
      }
    }

    m_poPtr = 0U;
  }

  if (m_poLen > 0U) {
    uint16_t space = io.getSpace();
    
    while (space > (4U * YSF_RADIO_SYMBOL_LENGTH)) {
      uint8_t c = m_poBuffer[m_poPtr++];
      writeByte(c);

      space -= 4U * YSF_RADIO_SYMBOL_LENGTH;
      
      if (m_poPtr >= m_poLen) {
        m_poPtr = 0U;
        m_poLen = 0U;
        return;
      }
    }
  }
}

uint8_t CYSFTX::writeData(const uint8_t* data, uint8_t length)
{
  if (length != (YSF_FRAME_LENGTH_BYTES + 1U))
    return 4U;

  uint16_t space = m_buffer.getSpace();
  if (space < YSF_FRAME_LENGTH_BYTES)
    return 5U;

  for (uint8_t i = 0U; i < YSF_FRAME_LENGTH_BYTES; i++)
    m_buffer.put(data[i + 1U]);

  return 0U;
}

void CYSFTX::writeByte(uint8_t c)
{
  q15_t inBuffer[YSF_RADIO_SYMBOL_LENGTH * 4U + 1U];
  q15_t outBuffer[YSF_RADIO_SYMBOL_LENGTH * 4U + 1U];

  const uint8_t MASK = 0xC0U;

  q15_t* p = inBuffer;
  for (uint8_t i = 0U; i < 4U; i++, c <<= 2, p += YSF_RADIO_SYMBOL_LENGTH) {
    switch (c & MASK) {
      case 0xC0U:
        ::memcpy(p, YSF_LEVELA, YSF_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
        break;
      case 0x80U:
        ::memcpy(p, YSF_LEVELB, YSF_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
        break;
      case 0x00U:
        ::memcpy(p, YSF_LEVELC, YSF_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
        break;
      default:
        ::memcpy(p, YSF_LEVELD, YSF_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
        break;
    }
  }

  uint16_t blockSize = YSF_RADIO_SYMBOL_LENGTH * 4U;

  // Handle the case of the oscillator not being accurate enough
  if (m_sampleCount > 0U) {
    m_count += YSF_RADIO_SYMBOL_LENGTH * 4U;

    if (m_count >= m_sampleCount) {
      if (m_sampleInsert) {
        inBuffer[YSF_RADIO_SYMBOL_LENGTH * 4U] = inBuffer[YSF_RADIO_SYMBOL_LENGTH * 4U - 1U];
        blockSize++;
      } else {
        blockSize--;
      }

      m_count -= m_sampleCount;
    }
  }

  ::arm_fir_fast_q15(&m_modFilter, inBuffer, outBuffer, blockSize);

  io.write(STATE_YSF, outBuffer, blockSize);
}

void CYSFTX::setTXDelay(uint8_t delay)
{
  m_txDelay = 600U + uint16_t(delay) * 12U;        // 500ms + tx delay
}

uint8_t CYSFTX::getSpace() const
{
  return m_buffer.getSpace() / YSF_FRAME_LENGTH_BYTES;
}

