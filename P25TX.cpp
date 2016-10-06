/*
 *   Copyright (C) 2016 by Jonathan Naylor G4KLX
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
#include "P25TX.h"

#include "P25Defines.h"

#if defined(WIDE_C4FSK_FILTERS_TX)
// Generated using rcosdesign(0.2, 4, 5, 'sqrt') in MATLAB
static q15_t P25_C4FSK_FILTER[] = {688, -680, -2158, -3060, -2724, -775, 2684, 7041, 11310, 14425, 15565, 14425,
                                   11310, 7041, 2684, -775, -2724, -3060, -2158, -680, 688, 0};
const uint16_t P25_C4FSK_FILTER_LEN = 22U;
#else
// Generated using rcosdesign(0.2, 8, 5, 'sqrt') in MATLAB
static q15_t P25_C4FSK_FILTER[] = {401, 104, -340, -731, -847, -553, 112, 909, 1472, 1450, 683, -675, -2144, -3040, -2706, -770, 2667, 6995,
                                   11237, 14331, 15464, 14331, 11237, 6995, 2667, -770, -2706, -3040, -2144, -675, 683, 1450, 1472, 909, 112,
                                   -553, -847, -731, -340, 104, 401, 0};
const uint16_t P25_C4FSK_FILTER_LEN = 42U;
#endif

// Generated in MATLAB using the following commands, and then normalised for unity gain
// shape2 = 'Inverse-sinc Lowpass';
// d2 = fdesign.interpolator(2, shape2);  
// h2 = design(d2, 'SystemObject', true);
static q15_t P25_LP_FILTER[] = {170, 401, 340, -203, -715, -478, 281, 419, -440, -1002, -103, 1114, 528, -1389, -1520, 1108, 2674, -388, -4662,
                                -2132, 9168, 20241, 20241, 9168, -2132, -4662, -388, 2674, 1108, -1520, -1389, 528, 1114, -103, -1002, -440, 419,
                                281, -478, -715, -203, 340, 401, 170};
const uint16_t P25_LP_FILTER_LEN = 44U;

// XXX These will need setting correctly
const q15_t P25_LEVELA[] = { 539,  539,  539,  539,  539};
const q15_t P25_LEVELB[] = { 179,  179,  179,  179,  179};
const q15_t P25_LEVELC[] = {-179, -179, -179, -179, -179};
const q15_t P25_LEVELD[] = {-539, -539, -539, -539, -539};

const uint8_t P25_START_SYNC = 0x77U;

CP25TX::CP25TX() :
m_buffer(1500U),
m_modFilter(),
m_lpFilter(),
m_modState(),
m_lpState(),
m_poBuffer(),
m_poLen(0U),
m_poPtr(0U),
m_txDelay(240U),      // 200ms
m_count(0U)
{
  ::memset(m_modState, 0x00U, 70U * sizeof(q15_t));
  ::memset(m_lpState,  0x00U, 70U * sizeof(q15_t));

  m_modFilter.numTaps = P25_C4FSK_FILTER_LEN;
  m_modFilter.pState  = m_modState;
  m_modFilter.pCoeffs = P25_C4FSK_FILTER;

  m_lpFilter.numTaps = P25_LP_FILTER_LEN;
  m_lpFilter.pState  = m_lpState;
  m_lpFilter.pCoeffs = P25_LP_FILTER;
}

void CP25TX::process()
{
  if (m_buffer.getData() == 0U && m_poLen == 0U)
    return;

  if (m_poLen == 0U) {
    if (!m_tx) {
      m_count = 0U;

      for (uint16_t i = 0U; i < m_txDelay; i++)
        m_poBuffer[m_poLen++] = P25_START_SYNC;
    } else {
      uint8_t length = m_buffer.get();
      for (uint8_t i = 0U; i < length; i++) {
        uint8_t c = m_buffer.get();
        m_poBuffer[m_poLen++] = c;
      }
    }

    m_poPtr = 0U;
  }

  if (m_poLen > 0U) {
    uint16_t space = io.getSpace();
    
    while (space > (4U * P25_RADIO_SYMBOL_LENGTH)) {
      uint8_t c = m_poBuffer[m_poPtr++];
      writeByte(c);

      space -= 4U * P25_RADIO_SYMBOL_LENGTH;
      
      if (m_poPtr >= m_poLen) {
        m_poPtr = 0U;
        m_poLen = 0U;
        return;
      }
    }
  }
}

uint8_t CP25TX::writeData(const uint8_t* data, uint8_t length)
{
  if (length < (P25_TERM_FRAME_LENGTH_BYTES + 1U))
    return 4U;

  uint16_t space = m_buffer.getSpace();
  if (space < length)
    return 5U;

  m_buffer.put(length - 1U);
  for (uint8_t i = 0U; i < (length - 1U); i++)
    m_buffer.put(data[i + 1U]);

  return 0U;
}

void CP25TX::writeByte(uint8_t c)
{
  q15_t inBuffer[P25_RADIO_SYMBOL_LENGTH * 4U + 1U];
  q15_t intBuffer[P25_RADIO_SYMBOL_LENGTH * 4U + 1U];
  q15_t outBuffer[P25_RADIO_SYMBOL_LENGTH * 4U + 1U];

  const uint8_t MASK = 0xC0U;

  q15_t* p = inBuffer;
  for (uint8_t i = 0U; i < 4U; i++, c <<= 2, p += P25_RADIO_SYMBOL_LENGTH) {
    switch (c & MASK) {
      case 0xC0U:
        ::memcpy(p, P25_LEVELA, P25_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
        break;
      case 0x80U:
        ::memcpy(p, P25_LEVELB, P25_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
        break;
      case 0x00U:
        ::memcpy(p, P25_LEVELC, P25_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
        break;
      default:
        ::memcpy(p, P25_LEVELD, P25_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
        break;
    }
  }

  uint16_t blockSize = P25_RADIO_SYMBOL_LENGTH * 4U;

  // Handle the case of the oscillator not being accurate enough
  if (m_sampleCount > 0U) {
    m_count += P25_RADIO_SYMBOL_LENGTH * 4U;

    if (m_count >= m_sampleCount) {
      if (m_sampleInsert) {
        inBuffer[P25_RADIO_SYMBOL_LENGTH * 4U] = inBuffer[P25_RADIO_SYMBOL_LENGTH * 4U - 1U];
        blockSize++;
      } else {
        blockSize--;
      }

      m_count -= m_sampleCount;
    }
  }

  ::arm_fir_fast_q15(&m_modFilter, inBuffer, intBuffer, blockSize);

  ::arm_fir_fast_q15(&m_lpFilter, intBuffer, outBuffer, blockSize);

  io.write(STATE_P25, outBuffer, blockSize);
}

void CP25TX::setTXDelay(uint8_t delay)
{
  m_txDelay = 600U + uint16_t(delay) * 12U;        // 500ms + tx delay
}

uint16_t CP25TX::getSpace() const
{
  return m_buffer.getSpace() / P25_LDU_FRAME_LENGTH_BYTES;
}
