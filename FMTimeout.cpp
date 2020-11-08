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
#include "FMTimeout.h"

// 400 Hz sine wave at 24000 Hz sample rate
const q15_t BUSY_AUDIO[] = {0, 3426, 6813, 10126, 13328, 16384, 19261, 21926, 24351, 26510, 28378, 29935, 31164, 32052, 32588, 32767, 32588, 32052, 31164, 29935, 28378, 26510, 24351,
                            21926, 19261, 16384, 13328, 10126, 6813, 3425, 0, -3425, -6813, -10126, -13328, -16384, -19261, -21926, -24351, -26510, -28378, -29935, -31164, -32052,
                            -32588, -32768, -32588, -32052, -31164, -29935, -28378, -26510, -24351, -21926, -19261, -16384, -13328, -10126, -6813, -3425};
const uint8_t BUSY_AUDIO_LEN = 60U;

CFMTimeout::CFMTimeout() :
m_level(128 * 128),
m_running(false),
m_pos(0U),
m_n(0U)
{
}

void CFMTimeout::setParams(uint8_t level)
{
  m_level = q15_t(level * 5);
}

q15_t CFMTimeout::getAudio()
{
  q15_t sample = 0;
  if (!m_running)
    return sample;

  if (m_pos >= 12000U) {
    q31_t sample31 = BUSY_AUDIO[m_n] * m_level;
    sample = q15_t(__SSAT((sample31 >> 15), 16));

    m_n++;
    if (m_n >= BUSY_AUDIO_LEN)
      m_n = 0U;
  }

  m_pos++;
  if (m_pos >= 24000U)
    m_pos = 0U;

  return sample;
}

void CFMTimeout::start()
{
  m_running = true;
  m_pos = 0U;
  m_n   = 0U;
}

void CFMTimeout::stop()
{
  m_running = false;
}

#endif

