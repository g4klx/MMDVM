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

#include "Config.h"
#include "Globals.h"
#include "YSFTX.h"

#include "YSFDefines.h"

#if defined(USE_SHORTER_DMR_FILTER)
// Generated using rcosdesign(0.2, 4, 10, 'sqrt') in MATLAB
static q15_t   YSF_C4FSK_FILTER[] = {486, 39, -480, -1022, -1526, -1928, -2164, -2178, -1927, -1384, -548, 561, 1898, 3399, 4980, 6546, 7999, 9246, 10202, 10803, 11008, 10803, 10202, 9246,
                                     7999, 6546, 4980, 3399, 1898, 561, -548, -1384, -1927, -2178, -2164, -1928, -1526, -1022, -480, 39, 486, 0};
const uint16_t YSF_C4FSK_FILTER_LEN = 42U;
#else
// Generated using rcosdesign(0.2, 8, 10, 'sqrt') in MATLAB
static q15_t   YSF_C4FSK_FILTER[] = {283, 198, 73, -78, -240, -393, -517, -590, -599, -533, -391, -181, 79, 364, 643, 880, 1041, 1097, 1026, 819, 483, 39, -477, -1016, -1516, -1915, -2150,
                                     -2163, -1914, -1375, -545, 557, 1886, 3376, 4946, 6502, 7946, 9184, 10134, 10731, 10935, 10731, 10134, 9184, 7946, 6502, 4946, 3376, 1886, 557, -545,
                                     -1375, -1914, -2163, -2150, -1915, -1516, -1016, -477, 39, 483, 819, 1026, 1097, 1041, 880, 643, 364, 79, -181, -391, -533, -599, -590, -517, -393, -240,
                                     -78, 73, 198, 283, 0};
const uint16_t YSF_C4FSK_FILTER_LEN = 82U;
#endif

const uint8_t YSF_START_SYNC = 0x77U;
const uint8_t YSF_END_SYNC   = 0xFFU;

q15_t YSF_A[] = { 1408,  1408,  1408,  1408,  1408,  1408,  1408,  1408,  1408,  1408};
q15_t YSF_B[] = {  470,   470,   470,   470,   470,   470,   470,   470,   470,   470};
q15_t YSF_C[] = { -470,  -470,  -470,  -470,  -470,  -470,  -470,  -470,  -470,  -470};
q15_t YSF_D[] = {-1408, -1408, -1408, -1408, -1408, -1408, -1408, -1408, -1408, -1408};


CYSFTX::CYSFTX() :
m_buffer(),
m_modFilter(),
m_modState(),
m_poBuffer(),
m_poLen(0U),
m_poPtr(0U),
m_txDelay(120U),      // 100ms
m_count(0U)
{
  ::memset(m_modState, 0x00U, 130U * sizeof(q15_t));

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
    
    while (space > (4U * YSF_RADIO_SYMBOL_LENGTH) && space < 1000U) {
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
        ::memcpy(p, YSF_A, YSF_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
        break;
      case 0x80U:
        ::memcpy(p, YSF_B, YSF_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
        break;
      case 0x00U:
        ::memcpy(p, YSF_C, YSF_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
        break;
      default:
        ::memcpy(p, YSF_D, YSF_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
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

  io.write(outBuffer, blockSize);
}

void CYSFTX::setTXDelay(uint8_t delay)
{
  m_txDelay = 240U + uint16_t(delay) * 12U;        // 200ms + tx delay
}

uint16_t CYSFTX::getSpace() const
{
  return m_buffer.getSpace() / YSF_FRAME_LENGTH_BYTES;
}

