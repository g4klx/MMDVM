/*
 *   Copyright (C) 2020 by Jonathan Naylor G4KLX
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

#if defined(MODE_FM)

#include "Globals.h"
#include "FMKeyer.h"

const struct {
  char     c;
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
  {'.', 0xBAEB8000U, 20U},
  {' ', 0x00000000U, 4U},
  {0U,  0x00000000U, 0U}
};

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT_FM(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])
#define READ_BIT_FM(p,i)    (p[(i)>>3] & BIT_MASK_TABLE[(i)&7])

CFMKeyer::CFMKeyer() :
m_wanted(false),
m_poBuffer(),
m_poLen(0U),
m_poPos(0U),
m_dotLen(0U),
m_dotPos(0U),
m_audio(NULL),
m_audioLen(0U),
m_audioPos(0U),
m_highLevel(0U),
m_lowLevel(0)
{
}

uint8_t CFMKeyer::setParams(const char* text, uint8_t speed, uint16_t frequency, uint8_t highLevel, uint8_t lowLevel)
{
  m_poLen = 0U;

  for (uint8_t i = 0U; text[i] != '\0'; i++) {
    for (uint8_t j = 0U; SYMBOL_LIST[j].c != 0U; j++) {
      if (SYMBOL_LIST[j].c == text[i]) {
        uint32_t MASK = 0x80000000U;
        for (uint8_t k = 0U; k < SYMBOL_LIST[j].length; k++, m_poLen++, MASK >>= 1) {
          bool b = (SYMBOL_LIST[j].pattern & MASK) == MASK;
          WRITE_BIT_FM(m_poBuffer, m_poLen, b);

          if (m_poLen >= 995U) {
            m_poLen = 0U;
            return 4U;
          }
        }

        break;
      }
    }
  }

  m_highLevel = q15_t(highLevel);
  m_lowLevel  = q15_t(lowLevel);

  m_dotLen = 24000U / speed;   // In samples

  m_audioLen = 24000U / frequency; // In samples

  delete[] m_audio;
  m_audio = new bool[m_audioLen];

  for (uint16_t i = 0U; i < m_audioLen; i++) {
    if (i < (m_audioLen / 2U))
      m_audio[i] = true;
    else
      m_audio[i] = false;
  }

  return 0U;
}

q15_t CFMKeyer::getHighAudio()
{
  q15_t output = 0U;
  if (!m_wanted)
    return 0U;

  bool b = READ_BIT_FM(m_poBuffer, m_poPos);
  if (b)
    output = m_audio[m_audioPos] ? m_highLevel : -m_highLevel;

  m_audioPos++;
  if (m_audioPos >= m_audioLen)
    m_audioPos = 0U;
  m_dotPos++;
  if (m_dotPos >= m_dotLen) {
    m_dotPos = 0U;
    m_poPos++;
    if (m_poPos >= m_poLen) {
      stop();
      return output;
    }
  }

  return output;
}

q15_t CFMKeyer::getLowAudio()
{
  q15_t output = 0U;
  if (!m_wanted)
    return 0U;

  bool b = READ_BIT_FM(m_poBuffer, m_poPos);
  if (b)
    output = m_audio[m_audioPos] ? m_lowLevel : -m_lowLevel;

  m_audioPos++;
  if (m_audioPos >= m_audioLen)
    m_audioPos = 0U;
  m_dotPos++;
  if (m_dotPos >= m_dotLen) {
    m_dotPos = 0U;
    m_poPos++;
    if (m_poPos >= m_poLen) {
      stop();
      return output;
    }
  }

  return output;
}

void CFMKeyer::start()
{
  if (isRunning())
    return;

  m_wanted   = true;
  m_poPos    = 0U;
  m_dotPos   = 0U;
  m_audioPos = 0U;
}

void CFMKeyer::stop()
{
  m_wanted   = false;
  m_poPos    = 0U;
  m_dotPos   = 0U;
  m_audioPos = 0U;
}

bool CFMKeyer::isWanted() const
{
  return m_wanted;
}

bool CFMKeyer::isRunning() const
{
  return m_poPos > 0U || m_dotPos > 0U || m_audioPos > 0U;
}

#endif

