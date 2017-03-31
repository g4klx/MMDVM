/*
 *   Copyright (C) 2009-2017 by Jonathan Naylor G4KLX
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

#if defined(WIDE_C4FSK_FILTERS_TX)
// Generated using rcosdesign(0.2, 4, 5, 'sqrt') in MATLAB
static q15_t DMR_C4FSK_FILTER[] = {688, -680, -2158, -3060, -2724, -775, 2684, 7041, 11310, 14425, 15565, 14425,
                                   11310, 7041, 2684, -775, -2724, -3060, -2158, -680, 688, 0};
const uint16_t DMR_C4FSK_FILTER_LEN = 22U;
#else
// Generated using rcosdesign(0.2, 8, 5, 'sqrt') in MATLAB
static q15_t DMR_C4FSK_FILTER[] = {401, 104, -340, -731, -847, -553, 112, 909, 1472, 1450, 683, -675, -2144, -3040, -2706, -770, 2667, 6995,
                                   11237, 14331, 15464, 14331, 11237, 6995, 2667, -770, -2706, -3040, -2144, -675, 683, 1450, 1472, 909, 112,
                                   -553, -847, -731, -340, 104, 401, 0};
const uint16_t DMR_C4FSK_FILTER_LEN = 42U;
#endif

const q15_t DMR_LEVELA[] = { 3195,  0 , 0,  0,  0};
const q15_t DMR_LEVELB[] = { 1065,  0 , 0,  0,  0};
const q15_t DMR_LEVELC[] = {-1065,  0 , 0,  0,  0};
const q15_t DMR_LEVELD[] = {-3195,  0 , 0,  0,  0};

const uint8_t CACH_INTERLEAVE[] =
      {1U,   2U,  3U,  5U,  6U,  7U,  9U, 10U, 11U, 13U, 15U, 16U, 17U, 19U, 20U, 21U, 23U,
       25U, 26U, 27U, 29U, 30U, 31U, 33U, 34U, 35U, 37U, 39U, 40U, 41U, 43U, 44U, 45U, 47U,
       49U, 50U, 51U, 53U, 54U, 55U, 57U, 58U, 59U, 61U, 63U, 64U, 65U, 67U, 68U, 69U, 71U,
       73U, 74U, 75U, 77U, 78U, 79U, 81U, 82U, 83U, 85U, 87U, 88U, 89U, 91U, 92U, 93U, 95U};

const uint8_t EMPTY_SHORT_LC[] =
      {0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U};

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])
#define READ_BIT1(p,i)    (p[(i)>>3] & BIT_MASK_TABLE[(i)&7])

const uint8_t DMR_SYNC = 0x77U;

CDMRDMOTX::CDMRDMOTX() :
m_fifo(),
m_modFilter(),
m_modState(),
m_poBuffer(),
m_poLen(0U),
m_poPtr(0U),
m_txDelay(240U),      // 200ms
m_cachPtr(0U)
{
  ::memset(m_modState, 0x00U, 70U * sizeof(q15_t));

  m_modFilter.numTaps = DMR_C4FSK_FILTER_LEN;
  m_modFilter.pState  = m_modState;
  m_modFilter.pCoeffs = DMR_C4FSK_FILTER;
}

void CDMRDMOTX::process()
{
  if (m_poLen == 0U && m_fifo.getData() > 0U) {
    if (!m_tx) {
      for (uint16_t i = 0U; i < m_txDelay; i++)
        m_poBuffer[i] = DMR_SYNC;

      m_poLen = m_txDelay;
    } else {
      createCACH(m_poBuffer + 0U,  0U);
      createCACH(m_poBuffer + 36U, 1U);

      for (unsigned int i = 0U; i < DMR_FRAME_LENGTH_BYTES; i++)
        m_poBuffer[i + 3U] = m_poBuffer[i + 39U] = m_fifo.get();

      m_poLen = 72U;
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
  q15_t inBuffer[DMR_RADIO_SYMBOL_LENGTH * 4U];
  q15_t outBuffer[DMR_RADIO_SYMBOL_LENGTH * 4U];

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

  ::arm_fir_fast_q15(&m_modFilter, inBuffer, outBuffer, DMR_RADIO_SYMBOL_LENGTH * 4U);

  io.write(STATE_DMR, outBuffer, DMR_RADIO_SYMBOL_LENGTH * 4U);
}

uint8_t CDMRDMOTX::getSpace() const
{
  return m_fifo.getSpace() / (DMR_FRAME_LENGTH_BYTES + 2U);
}

void CDMRDMOTX::setTXDelay(uint8_t delay)
{
  m_txDelay = 240U + uint16_t(delay) * 12U;        // 200ms + tx delay
}

void CDMRDMOTX::createCACH(uint8_t* buffer, uint8_t slotIndex)
{
  if (m_cachPtr >= 12U)
    m_cachPtr = 0U;

  ::memcpy(buffer, EMPTY_SHORT_LC + m_cachPtr, 3U);

  bool at  = true;
  bool tc  = slotIndex == 1U;
  bool ls0 = true;            // For 1 and 2
  bool ls1 = true;

  if (m_cachPtr == 0U)          // For 0
    ls1 = false;
  else if (m_cachPtr == 9U)     // For 3
    ls0 = false;

  bool h0 = at ^ tc ^ ls1;
  bool h1 = tc ^ ls1 ^ ls0;
  bool h2 = at ^ tc       ^ ls0;

  buffer[0U] |= at ? 0x80U : 0x00U;
  buffer[0U] |= tc ? 0x08U : 0x00U;
  buffer[1U] |= ls1 ? 0x80U : 0x00U;
  buffer[1U] |= ls0 ? 0x08U : 0x00U;
  buffer[1U] |= h0 ? 0x02U : 0x00U;
  buffer[2U] |= h1 ? 0x20U : 0x00U;
  buffer[2U] |= h2 ? 0x02U : 0x00U;

  m_cachPtr += 3U;
}
