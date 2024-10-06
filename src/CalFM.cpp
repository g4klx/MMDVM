/*
 *   Copyright (C) 2009-2015,2020 by Jonathan Naylor G4KLX
 *   Copyright (C) 2016 by Colin Durbridge G4EML
 *   Copyright (C) 2020 by Phil Taylor M0VSE
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
#include "CalFM.h"


const struct TONE_TABLE {
  uint16_t  frequency;
  uint16_t length;
  q31_t    increment;
} TONE_TABLE_DATA[] = {
  {2495U, 10U, 223248821},
  {2079U, 12U, 186025772},
  {1633U, 15U, 146118367},
  {1247U, 19U, 111579672},
  {1039U, 23U, 93012886},
  {956U,  25U, 85541432}};

const uint8_t TONE_TABLE_DATA_LEN = 6U;

CCalFM::CCalFM() :
m_frequency(0),
m_length(0),
m_tone(NULL),
m_level(128 * 12),
m_transmit(false),
m_audioSeq(0),
m_lastState(STATE_IDLE)
{
}

void CCalFM::process()
{
  const TONE_TABLE* entry = NULL;

  if (m_modemState != m_lastState)
  {
    switch (m_modemState) {
        case STATE_FMCAL10K:
          m_frequency = 956U;
          break;
        case STATE_FMCAL12K:
          m_frequency = 1039U;
          break;
        case STATE_FMCAL15K:
          m_frequency = 1247U;
          break;
        case STATE_FMCAL20K:
          m_frequency = 1633U;
          break;
        case STATE_FMCAL25K:
          m_frequency = 2079U;
          break;
        case STATE_FMCAL30K:
          m_frequency = 2495U;
          break;
        default:
          m_frequency = 0U;
          break;
    }

    for (uint8_t i = 0U; i < TONE_TABLE_DATA_LEN; i++) {
      if (TONE_TABLE_DATA[i].frequency == m_frequency) {
        entry = TONE_TABLE_DATA + i;
        break;
      }
    }

    if (entry == NULL)
      return;

    m_length = entry->length;

    delete[] m_tone;
    m_tone = new q15_t[m_length];

    q31_t arg = 0;
    for (uint16_t i = 0U; i < m_length; i++) {
      q63_t value = ::arm_sin_q31(arg) * q63_t(m_level);
      m_tone[i] = q15_t(__SSAT((value >> 31), 16));

      arg += entry->increment;
    }

    m_lastState=m_modemState;
  }

  if (m_transmit)
  {
    uint16_t space = io.getSpace();
    while (space > m_length)
    {
      io.write(m_modemState,m_tone,m_length);
      space -= m_length;
    }
  }
}


uint8_t CCalFM::write(const uint8_t* data, uint16_t length)
{
  if (length != 1U)
    return 4U;

  m_transmit = data[0U] == 1U;

  return 0U;
}

#endif

