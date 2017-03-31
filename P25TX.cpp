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

// #define WANT_DEBUG

#include "Config.h"
#include "Globals.h"
#include "P25TX.h"

#include "P25Defines.h"

#if defined(WIDE_C4FSK_FILTERS_TX)
// Generated using rcosdesign(0.2, 4, 5, 'normal') in MATLAB
static q15_t P25_C4FSK_FILTER[] = {-1392, -2602, -3043, -2238, 0, 3460, 7543, 11400, 14153, 15152, 14153,	11400, 7543, 3460, 0, -2238, -3043, -2602, -1392, 0};
const uint16_t P25_C4FSK_FILTER_LEN = 20U;
#else
// Generated using rcosdesign(0.2, 8, 5, 'normal') in MATLAB
static q15_t P25_C4FSK_FILTER[] = {-413, -751, -845, -587, 0, 740, 1348, 1520, 1063, 0, -1383, -2583, -3021, -2222, 0, 3435, 7488, 11318, 14053, 15044, 14053,
                                   11318, 7488, 3435, 0, -2222, -3021, -2583, -1383, 0, 1063, 1520, 1348, 740, 0, -587, -845, -751, -413, 0};
const uint16_t P25_C4FSK_FILTER_LEN = 40U;
#endif

// Generated in MATLAB using the following commands, and then normalised for unity gain
// shape2 = 'Inverse-sinc Lowpass';
// d2 = fdesign.interpolator(2, shape2);  
// h2 = design(d2, 'SystemObject', true);
static q15_t P25_LP_FILTER[] = {170, 401, 340, -203, -715, -478, 281, 419, -440, -1002, -103, 1114, 528, -1389, -1520, 1108, 2674, -388, -4662,
                                -2132, 9168, 20241, 20241, 9168, -2132, -4662, -388, 2674, 1108, -1520, -1389, 528, 1114, -103, -1002, -440, 419,
                                281, -478, -715, -203, 340, 401, 170};
const uint16_t P25_LP_FILTER_LEN = 44U;

const q15_t P25_LEVELA[] = { 2475,  0,  0,  0,  0};
const q15_t P25_LEVELB[] = { 825,  0,  0,  0,  0};
const q15_t P25_LEVELC[] = {-825,  0,  0,  0,  0};
const q15_t P25_LEVELD[] = {-2475,  0,  0,  0,  0};

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
m_txDelay(240U)       // 200ms
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
  q15_t inBuffer[P25_RADIO_SYMBOL_LENGTH * 4U];
  q15_t intBuffer[P25_RADIO_SYMBOL_LENGTH * 4U];
  q15_t outBuffer[P25_RADIO_SYMBOL_LENGTH * 4U];

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

  ::arm_fir_fast_q15(&m_modFilter, inBuffer, intBuffer, P25_RADIO_SYMBOL_LENGTH * 4U);

  ::arm_fir_fast_q15(&m_lpFilter, intBuffer, outBuffer, P25_RADIO_SYMBOL_LENGTH * 4U);

  io.write(STATE_P25, outBuffer, P25_RADIO_SYMBOL_LENGTH * 4U);
}

void CP25TX::setTXDelay(uint8_t delay)
{
  m_txDelay = 600U + uint16_t(delay) * 12U;        // 500ms + tx delay
}

uint8_t CP25TX::getSpace() const
{
  return m_buffer.getSpace() / P25_LDU_FRAME_LENGTH_BYTES;
}
