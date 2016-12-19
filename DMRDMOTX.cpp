/*
 *   Copyright (C) 2009-2016 by Jonathan Naylor G4KLX
 *   Copyright (C) 2016 by Colin Durbridge G4EML
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
#include "DMRSlotType.h"

// Generated using rcosdesign(0.2, 4, 10, 'sqrt') in MATLAB
static q15_t   DMR_C4FSK_FILTER[] = {486, 39, -480, -1022, -1526, -1928, -2164, -2178, -1927, -1384, -548, 561, 1898, 3399, 4980, 6546, 7999, 9246, 10202, 10803, 11008, 10803, 10202, 9246,
                                 7999, 6546, 4980, 3399, 1898, 561, -548, -1384, -1927, -2178, -2164, -1928, -1526, -1022, -480, 39, 486, 0};
const uint16_t DMR_C4FSK_FILTER_LEN = 42U;

const q15_t DMR_LEVELA[] = { 395,  395,  395,  395,  395,  395,  395,  395,  395,  395};
const q15_t DMR_LEVELB[] = { 131,  131,  131,  131,  131,  131,  131,  131,  131,  131};
const q15_t DMR_LEVELC[] = {-131, -131, -131, -131, -131, -131, -131, -131, -131, -131};
const q15_t DMR_LEVELD[] = {-395, -395, -395, -395, -395, -395, -395, -395, -395, -395};


CDMRDMOTX::CDMRDMOTX() :
m_fifo(),
m_modFilter(),
m_modState(),
m_poBuffer(),
m_poLen(0U),
m_poPtr(0U),
m_txDelay(240U),      // 200ms
m_count(0U)
{
  ::memset(m_modState, 0x00U, 90U * sizeof(q15_t));

  m_modFilter.numTaps = DMR_C4FSK_FILTER_LEN;
  m_modFilter.pState  = m_modState;
  m_modFilter.pCoeffs = DMR_C4FSK_FILTER;
}

void CDMRDMOTX::process()
{
  if (m_poLen == 0U && m_fifo.getData() > 0U) {
    if (!m_tx) {
      for (uint16_t i = 0U; i < m_txDelay; i++)
        m_poBuffer[m_poLen++] = 0x00U;
    } else {
      for (unsigned int i = 0U; i < 72U; i++)
        m_poBuffer[m_poLen++] = 0x00U;

      for (unsigned int i = 0U; i < DMR_FRAME_LENGTH_BYTES; i++)
        m_poBuffer[i] = m_fifo.get();
    }

    m_poPtr = 0U;
  }

  if (m_poLen > 0U) {
    uint16_t space = io.getSpace();
    
    while (space > (4U * DMR_RADIO_SYMBOL_LENGTH)) {
      uint8_t c = m_poBuffer[m_poPtr++];

      writeByte(c);

      space -= 4U * DMR_RADIO_SYMBOL_LENGTH;
      
      if (m_poPtr >= m_poLen) {
        m_poPtr = 0U;
        m_poLen = 0U;
        return;
      }
    }
  }
}

uint8_t CDMRDMOTX::writeData(const uint8_t* data, uint8_t length)
{
  if (length != (DMR_FRAME_LENGTH_BYTES + 1U))
    return 4U;

  uint16_t space = m_fifo.getSpace();
  if (space < DMR_FRAME_LENGTH_BYTES)
    return 5U;

  for (uint8_t i = 0U; i < DMR_FRAME_LENGTH_BYTES; i++)
    m_fifo.put(data[i + 1U]);

  return 0U;
}

void CDMRDMOTX::writeByte(uint8_t c)
{
  q15_t inBuffer[DMR_RADIO_SYMBOL_LENGTH * 4U + 1U];
  q15_t outBuffer[DMR_RADIO_SYMBOL_LENGTH * 4U + 1U];

  const uint8_t MASK = 0xC0U;

  q15_t* p = inBuffer;
  for (uint8_t i = 0U; i < 4U; i++, c <<= 2, p += DMR_RADIO_SYMBOL_LENGTH) {
    switch (c & MASK) {
      case 0xC0U:
        ::memcpy(p, DMR_LEVELA, DMR_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
        break;
      case 0x80U:
        ::memcpy(p, DMR_LEVELB, DMR_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
        break;
      case 0x00U:
        ::memcpy(p, DMR_LEVELC, DMR_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
        break;
      default:
        ::memcpy(p, DMR_LEVELD, DMR_RADIO_SYMBOL_LENGTH * sizeof(q15_t));
        break;
    }
  }

  uint16_t blockSize = DMR_RADIO_SYMBOL_LENGTH * 4U;

  // Handle the case of the oscillator not being accurate enough
  if (m_sampleCount > 0U) {
    m_count += DMR_RADIO_SYMBOL_LENGTH * 4U;

    if (m_count >= m_sampleCount) {
      if (m_sampleInsert) {
        inBuffer[DMR_RADIO_SYMBOL_LENGTH * 4U] = inBuffer[DMR_RADIO_SYMBOL_LENGTH * 4U - 1U];
        blockSize++;
      } else {
        blockSize--;
      }

      m_count -= m_sampleCount;
    }
  }

  ::arm_fir_fast_q15(&m_modFilter, inBuffer, outBuffer, blockSize);

  io.write(STATE_DMR, outBuffer, blockSize);
}

uint16_t CDMRDMOTX::getSpace() const
{
  return m_fifo.getSpace() / (DMR_FRAME_LENGTH_BYTES + 2U);
}

void CDMRDMOTX::setTXDelay(uint8_t delay)
{
  m_txDelay = 600U + uint16_t(delay) * 12U;        // 500ms + tx delay
}

