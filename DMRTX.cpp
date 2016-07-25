/*
 *   Copyright (C) 2009-2016 by Jonathan Naylor G4KLX
 *   Copyright (C) 2016 by Colin Durbridge G4EML
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.
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

const q15_t DMR_LEVELA[] = { 640,  640 , 640,  640,  640};
const q15_t DMR_LEVELB[] = { 213,  213,  213,  213,  213};
const q15_t DMR_LEVELC[] = {-213, -213, -213, -213, -213};
const q15_t DMR_LEVELD[] = {-640, -640, -640, -640, -640};

// The PR FILL and Data Sync pattern.
const uint8_t IDLE_DATA[] =
        {0x53U, 0xC2U, 0x5EU, 0xABU, 0xA8U, 0x67U, 0x1DU, 0xC7U, 0x38U, 0x3BU, 0xD9U,
         0x36U, 0x00U, 0x0DU, 0xFFU, 0x57U, 0xD7U, 0x5DU, 0xF5U, 0xD0U, 0x03U, 0xF6U,
         0xE4U, 0x65U, 0x17U, 0x1BU, 0x48U, 0xCAU, 0x6DU, 0x4FU, 0xC6U, 0x10U, 0xB4U};

const uint8_t CACH_INTERLEAVE[] =
        { 1U,  2U,  3U,  5U,  6U,  7U,  9U, 10U, 11U, 13U, 15U, 16U, 17U, 19U, 20U, 21U, 23U,
         25U, 26U, 27U, 29U, 30U, 31U, 33U, 34U, 35U, 37U, 39U, 40U, 41U, 43U, 44U, 45U, 47U,
         49U, 50U, 51U, 53U, 54U, 55U, 57U, 58U, 59U, 61U, 63U, 64U, 65U, 67U, 68U, 69U, 71U,
         73U, 74U, 75U, 77U, 78U, 79U, 81U, 82U, 83U, 85U, 87U, 88U, 89U, 91U, 92U, 93U, 95U};

const uint8_t EMPTY_SHORT_LC[] = 
        {0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U};

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])
#define READ_BIT1(p,i)    (p[(i)>>3] & BIT_MASK_TABLE[(i)&7])

CDMRTX::CDMRTX() :
m_fifo(),
m_modFilter(),
m_modState(),
m_state(DMRTXSTATE_IDLE),
m_idle(),
m_cachPtr(0U),
m_shortLC(),
m_newShortLC(),
m_markBuffer(),
m_poBuffer(),
m_poLen(0U),
m_poPtr(0U),
m_count(0U),
m_abort()
{
  ::memset(m_modState, 0x00U, 70U * sizeof(q15_t));

  m_modFilter.numTaps = DMR_C4FSK_FILTER_LEN;
  m_modFilter.pState  = m_modState;
  m_modFilter.pCoeffs = DMR_C4FSK_FILTER;

  ::memcpy(m_newShortLC, EMPTY_SHORT_LC, 12U);
  ::memcpy(m_shortLC,    EMPTY_SHORT_LC, 12U);

  m_abort[0U] = false;
  m_abort[1U] = false;
}

void CDMRTX::process()
{
  if (m_state == DMRTXSTATE_IDLE)
    return;

  if (m_poLen == 0U) {
    switch (m_state) {
      case DMRTXSTATE_SLOT1:
        createData(0U);
        m_state = DMRTXSTATE_CACH2;
        break;

      case DMRTXSTATE_CACH2:
        createCACH(1U, 0U);
        m_state = DMRTXSTATE_SLOT2;
        break;

      case DMRTXSTATE_SLOT2:
        createData(1U);
        m_state = DMRTXSTATE_CACH1;
        break;

      case DMRTXSTATE_CAL:
        createCal();
        break;

      default:
        createCACH(0U, 1U);
        m_state = DMRTXSTATE_SLOT1;
        break;
    }
  }

  if (m_poLen > 0U) {
    uint16_t space = io.getSpace();
    
    while (space > (4U * DMR_RADIO_SYMBOL_LENGTH)) {
      uint8_t c = m_poBuffer[m_poPtr];
      uint8_t m = m_markBuffer[m_poPtr];
      m_poPtr++;

      writeByte(c, m);

      space -= 4U * DMR_RADIO_SYMBOL_LENGTH;
      
      if (m_poPtr >= m_poLen) {
        m_poPtr = 0U;
        m_poLen = 0U;
        return;
      }
    }
  }
}

uint8_t CDMRTX::writeData1(const uint8_t* data, uint8_t length)
{
  if (length != (DMR_FRAME_LENGTH_BYTES + 1U))
    return 4U;

  uint16_t space = m_fifo[0U].getSpace();
  if (space < DMR_FRAME_LENGTH_BYTES)
    return 5U;

  if (m_abort[0U]) {
    m_fifo[0U].reset();
    m_abort[0U] = false;
  }

  for (uint8_t i = 0U; i < DMR_FRAME_LENGTH_BYTES; i++)
    m_fifo[0U].put(data[i + 1U]);

  // Start the TX if it isn't already on
  if (!m_tx)
    m_state = DMRTXSTATE_SLOT1;

  return 0U;
}

uint8_t CDMRTX::writeData2(const uint8_t* data, uint8_t length)
{
  if (length != (DMR_FRAME_LENGTH_BYTES + 1U))
    return 4U;

  uint16_t space = m_fifo[1U].getSpace();
  if (space < DMR_FRAME_LENGTH_BYTES)
    return 5U;

  if (m_abort[1U]) {
    m_fifo[1U].reset();
    m_abort[1U] = false;
  }

  for (uint8_t i = 0U; i < DMR_FRAME_LENGTH_BYTES; i++)
    m_fifo[1U].put(data[i + 1U]);

  // Start the TX if it isn't already on
  if (!m_tx)
    m_state = DMRTXSTATE_SLOT1;

  return 0U;
}

uint8_t CDMRTX::writeShortLC(const uint8_t* data, uint8_t length)
{
  if (length != 9U)
    return 4U;

  ::memset(m_newShortLC, 0x00U, 12U);
  
  for (uint8_t i = 0U; i < 68U; i++) {
    bool b = READ_BIT1(data, i);
    uint8_t n = CACH_INTERLEAVE[i];
    WRITE_BIT1(m_newShortLC, n, b);
  }

  return 0U;
}

uint8_t CDMRTX::writeAbort(const uint8_t* data, uint8_t length)
{
  if (length != 1U)
    return 4U;

  switch (data[0U]) {
    case 1U:
      m_abort[0U] = true;
      return 0U;

    case 2U:
      m_abort[1U] = true;
      return 0U;

    default:
      return 4U;
  }
}

void CDMRTX::setStart(bool start)
{
  m_state = start ? DMRTXSTATE_SLOT1 : DMRTXSTATE_IDLE;

  m_count = 0U;

  m_abort[0U] = false;
  m_abort[1U] = false;
}

void CDMRTX::setCal(bool start)
{
  m_state = start ? DMRTXSTATE_CAL : DMRTXSTATE_IDLE;

  m_count = 0U;
}

void CDMRTX::writeByte(uint8_t c, uint8_t control)
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

  uint8_t controlBuffer[DMR_RADIO_SYMBOL_LENGTH * 4U + 1U];
  ::memset(controlBuffer, MARK_NONE, DMR_RADIO_SYMBOL_LENGTH * 4U * sizeof(uint8_t));
  controlBuffer[DMR_RADIO_SYMBOL_LENGTH * 2U] = control;  

  // Handle the case of the oscillator not being accurate enough
  if (m_sampleCount > 0U) {
    m_count += DMR_RADIO_SYMBOL_LENGTH * 4U;

    if (m_count >= m_sampleCount) {
      if (m_sampleInsert) {
        inBuffer[DMR_RADIO_SYMBOL_LENGTH * 4U] = inBuffer[DMR_RADIO_SYMBOL_LENGTH * 4U - 1U];
        for (int8_t i = DMR_RADIO_SYMBOL_LENGTH * 4U - 1; i >= 0; i--)
          controlBuffer[i + 1] = controlBuffer[i];
        blockSize++;
      } else {
        controlBuffer[DMR_RADIO_SYMBOL_LENGTH * 2U - 1U] = control;  
        for (uint8_t i = 0U; i < (DMR_RADIO_SYMBOL_LENGTH * 4U - 1U); i++)
          controlBuffer[i] = controlBuffer[i + 1U];
        blockSize--;
      }

      m_count -= m_sampleCount;
    }
  }

  ::arm_fir_fast_q15(&m_modFilter, inBuffer, outBuffer, blockSize);

  io.write(outBuffer, blockSize, controlBuffer);
}

uint16_t CDMRTX::getSpace1() const
{
  return m_fifo[0U].getSpace() / (DMR_FRAME_LENGTH_BYTES + 2U);
}

uint16_t CDMRTX::getSpace2() const
{
  return m_fifo[1U].getSpace() / (DMR_FRAME_LENGTH_BYTES + 2U);
}

void CDMRTX::createData(uint8_t slotIndex)
{
  if (m_fifo[slotIndex].getData()> 0U) {
    for (unsigned int i = 0U; i < DMR_FRAME_LENGTH_BYTES; i++) {
      m_poBuffer[i]   = m_fifo[slotIndex].get();
      m_markBuffer[i] = MARK_NONE;
    }
  } else {
    m_abort[slotIndex] = false;
    // Transmit an idle message
    for (unsigned int i = 0U; i < DMR_FRAME_LENGTH_BYTES; i++) {
      m_poBuffer[i]   = m_idle[i];
      m_markBuffer[i] = MARK_NONE;
    }
  }

  m_poLen = DMR_FRAME_LENGTH_BYTES;
  m_poPtr = 0U;
}

void CDMRTX::createCal()
{
  for (unsigned int i = 0U; i < DMR_FRAME_LENGTH_BYTES; i++) {
    m_poBuffer[i]   = 0x5FU;              // +3, +3, -3, -3 pattern for deviation cal.
    m_markBuffer[i] = MARK_NONE;
  }

  m_poLen = DMR_FRAME_LENGTH_BYTES;
  m_poPtr = 0U;
}

void CDMRTX::createCACH(uint8_t txSlotIndex, uint8_t rxSlotIndex)
{
  if (m_cachPtr >= 12U)
    m_cachPtr = 0U;

  if (m_cachPtr == 0U) {
    if (m_fifo[0U].getData() == 0U && m_fifo[1U].getData() == 0U)
      ::memcpy(m_shortLC, EMPTY_SHORT_LC, 12U);
    else
      ::memcpy(m_shortLC, m_newShortLC, 12U);
  }

  ::memcpy(m_poBuffer, m_shortLC + m_cachPtr, 3U);
  m_markBuffer[0U] = MARK_NONE;
  m_markBuffer[1U] = MARK_NONE;
  m_markBuffer[2U] = rxSlotIndex == 1U ? MARK_SLOT1 : MARK_SLOT2;

  bool at = m_fifo[rxSlotIndex].getData() > 0U;
  bool tc = txSlotIndex == 1U;
  bool ls0 = true;            // For 1 and 2
  bool ls1 = true;

  if (m_cachPtr == 0U)          // For 0
    ls1 = false;
  else if (m_cachPtr == 9U)     // For 3
    ls0 = false;

  bool h0 = at ^ tc ^ ls1;
  bool h1 =      tc ^ ls1 ^ ls0;
  bool h2 = at ^ tc       ^ ls0;

  m_poBuffer[0U] |= at  ? 0x80U : 0x00U;
  m_poBuffer[0U] |= tc  ? 0x08U : 0x00U;
  m_poBuffer[1U] |= ls1 ? 0x80U : 0x00U;
  m_poBuffer[1U] |= ls0 ? 0x08U : 0x00U;
  m_poBuffer[1U] |= h0  ? 0x02U : 0x00U;
  m_poBuffer[2U] |= h1  ? 0x20U : 0x00U;
  m_poBuffer[2U] |= h2  ? 0x02U : 0x00U;

  m_poLen = DMR_CACH_LENGTH_BYTES;
  m_poPtr = 0U;
  
  m_cachPtr += 3U;
}

void CDMRTX::setColorCode(uint8_t colorCode)
{
  ::memcpy(m_idle, IDLE_DATA, DMR_FRAME_LENGTH_BYTES);

  CDMRSlotType slotType;
  slotType.encode(colorCode, DT_IDLE, m_idle);
}

