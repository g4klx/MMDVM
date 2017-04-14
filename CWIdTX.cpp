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

#include "Config.h"
#include "Globals.h"
#include "CWIdTX.h"

q15_t TONE[] = {
    0, 518, 1000, 1414, 1732, 1932, 2000, 1932, 1732, 1414, 1000, 518, 0, -518, -1000, -1414, -1732, -1932, -2000, -1932, -1732, -1414, -1000, -518};

q15_t SILENCE[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const uint8_t CYCLE_LENGTH = 24U;

const uint8_t DOT_LENGTH = 50U;

const struct {
  uint8_t  c;
  uint32_t pattern;
  uint8_t  length;
} SYMBOL_LIST[] = {
  {'A', 0xB8000000U, 8U},
  {'B', 0xEA800000U, 12U},
  {'C', 0xEBA00000U, 14U},
  {'D', 0xEA000000U, 10U},
  {'E', 0x80000000U, 4U},
  {'F', 0xAE800000U, 12U},
  {'G', 0xEE800000U, 12U},
  {'H', 0xAA000000U, 10U},
  {'I', 0xA0000000U, 6U},
  {'J', 0xBBB80000U, 16U},
  {'K', 0xEB800000U, 12U},
  {'L', 0xBA800000U, 12U},
  {'M', 0xEE000000U, 10U},
  {'N', 0xE8000000U, 8U},
  {'O', 0xEEE00000U, 14U},
  {'P', 0xBBA00000U, 14U},
  {'Q', 0xEEB80000U, 16U},
  {'R', 0xBA000000U, 10U},
  {'S', 0xA8000000U, 8U},
  {'T', 0xE0000000U, 6U},
  {'U', 0xAE000000U, 10U},
  {'V', 0xAB800000U, 12U},
  {'W', 0xBB800000U, 12U},
  {'X', 0xEAE00000U, 14U},
  {'Y', 0xEBB80000U, 16U},
  {'Z', 0xEEA00000U, 14U},
  {'1', 0xBBBB8000U, 20U},
  {'2', 0xAEEE0000U, 18U},
  {'3', 0xABB80000U, 16U},
  {'4', 0xAAE00000U, 14U},
  {'5', 0xAA800000U, 12U},
  {'6', 0xEAA00000U, 14U},
  {'7', 0xEEA80000U, 16U},
  {'8', 0xEEEA0000U, 18U},
  {'9', 0xEEEE8000U, 20U},
  {'0', 0xEEEEE000U, 22U},
  {'/', 0xEAE80000U, 16U},
  {'?', 0xAEEA0000U, 18U},
  {',', 0xEEAEE000U, 22U},
  {'-', 0xEAAE0000U, 18U},
  {'=', 0xEAB80000U, 16U},
  {' ', 0x00000000U, 4U},
  {0U,  0x00000000U, 0U}
};

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])
#define READ_BIT1(p,i)    (p[(i)>>3] & BIT_MASK_TABLE[(i)&7])

CCWIdTX::CCWIdTX() :
m_poBuffer(),
m_poLen(0U),
m_poPtr(0U),
m_n(0U)
{
}

void CCWIdTX::process()
{
  if (m_poLen == 0U)
    return;

  uint16_t space = io.getSpace();
    
  while (space > CYCLE_LENGTH) {
    bool b = READ_BIT1(m_poBuffer, m_poPtr);
    if (b)
      io.write(STATE_CWID, TONE, CYCLE_LENGTH);
    else
      io.write(STATE_CWID, SILENCE, CYCLE_LENGTH);

    space -= CYCLE_LENGTH;

    m_n++;
    if (m_n >= DOT_LENGTH) {
      m_poPtr++;
      m_n = 0U;
    }

    if (m_poPtr >= m_poLen) {
      m_poPtr = 0U;
      m_poLen = 0U;
      return;
    }
  }
}

uint8_t CCWIdTX::write(const uint8_t* data, uint8_t length)
{
  ::memset(m_poBuffer, 0x00U, 1000U * sizeof(uint8_t));

  m_poLen = 8U;
  m_poPtr = 0U;
  m_n     = 0U;

  for (uint8_t i = 0U; i < length; i++) {
    for (uint8_t j = 0U; SYMBOL_LIST[j].c != 0U; j++) {
      if (SYMBOL_LIST[j].c == data[i]) {
        uint32_t MASK = 0x80000000U;
        for (uint8_t k = 0U; k < SYMBOL_LIST[j].length; k++, m_poLen++, MASK >>= 1) {
          bool b = (SYMBOL_LIST[j].pattern & MASK) == MASK;
          WRITE_BIT1(m_poBuffer, m_poLen, b);

          if (m_poLen >= 995U) {
            m_poLen = 0U;
            return 4U;
          }
        }

        break;
      }
    }
  }

  // An empty message
  if (m_poLen == 8U) {
    m_poLen = 0U;
    return 4U;
  }

  m_poLen += 5U;

  DEBUG2("Message created with length", m_poLen);

  return 0U;
}

void CCWIdTX::reset()
{
  m_poLen = 0U;
  m_poPtr = 0U;
  m_n     = 0U;
}

