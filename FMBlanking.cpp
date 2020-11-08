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
#include "FMBlanking.h"

// 2000 Hz sine wave at 24000 Hz sample rate
const q31_t TONE[] = {0, 16384, 28378, 32767, 28378, 16384, 0, -16383, -28377, -32767, -28377, -16383};

const uint8_t TONE_LEN = 12U;

const uint32_t BLEEP_LEN = 2400U;   // 100ms

CFMBlanking::CFMBlanking() :
m_posValue(0),
m_negValue(0),
m_level(128 * 128),
m_running(false),
m_pos(0U),
m_n(0U)
{
}

void CFMBlanking::setParams(uint8_t value, uint8_t level)
{
  m_posValue = q15_t(value * 128);
  m_negValue = -m_posValue;

  m_level = q15_t(level * 128);
}

q15_t CFMBlanking::process(q15_t sample)
{
  if (m_posValue == 0)
    return sample;

  if (!m_running) {
    if (sample >= m_posValue) {
      m_running = true;
      m_pos = 0U;
      m_n   = 0U;
    } else if (sample <= m_negValue) {
      m_running = true;
      m_pos = 0U;
      m_n   = 0U;
    }
  }

  if (!m_running)
    return sample;

  if (m_pos <= BLEEP_LEN) {
    q31_t value = TONE[m_n++] * m_level;
    sample = q15_t(__SSAT((value >> 15), 16));
    if (m_n >= TONE_LEN)
      m_n = 0U;
  } else {
    sample = 0;
  }

  m_pos++;
  if (m_pos >= 12000U)
    m_running = false;

  return sample;
}

#endif

