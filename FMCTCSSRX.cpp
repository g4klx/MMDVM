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
#include "Globals.h"
#include "FMCTCSSRX.h"

const struct CTCSS_TABLE {
  uint8_t frequency;
  long   coeff;
} CTCSS_TABLE_DATA[] = {
  {67, 65527},
{69, 65526},
{71, 65525},
{74, 65524},
{77, 65523},
{79, 65522},
{82, 65521},
{85, 65520},
{88, 65519},
{91, 65518},
{94, 65517},
{97, 65516},
{100, 65514},
{103, 65513},
{107, 65511},
{110, 65509},
{114, 65507},
{123, 65503},
{127, 65501},
{131, 65498},
{136, 65495},
{141, 65492},
{146, 65489},
{150, 65487},
{151, 65486},
{156, 65482},
{159, 65480},
{162, 65478},
{165, 65476},
{167, 65474},
{171, 65472},
{173, 65470},
{177, 65467},
{179, 65465},
{183, 65462},
{186, 65460},
{188, 65458},
{189, 65457},
{192, 65454},
{196, 65451},
{199, 65448},
{203, 65445},
{206, 65442},
{210, 65438},
{213, 65435},
{218, 65431},
{221, 65428},
{225, 65424},
{229, 65420},
{233, 65416},
{237, 65412},
{241, 65407},
{245, 65403},
{250, 65398},
{254, 65393}
};

const uint8_t CTCSS_TABLE_DATA_LEN = 55U;

// 4Hz bandwidth
const uint16_t N = 24000U / 4U;

CFMCTCSSRX::CFMCTCSSRX() :
m_coeff(0),
m_thresholdSquared(0U),
m_count(0U),
m_q0(0),
m_q1(0),
m_result(false)
{
}

uint8_t CFMCTCSSRX::setParams(uint8_t frequency, uint8_t threshold)
{
  for (uint8_t i = 0U; i < CTCSS_TABLE_DATA_LEN; i++) {
    if (CTCSS_TABLE_DATA[i].frequency == frequency) {
      m_coeff = CTCSS_TABLE_DATA[i].coeff;
      break;
    }
  }

  if (m_coeff == 0)
    return 4U;

  m_thresholdSquared = (long)(threshold * threshold);

  return 0U;
}

bool CFMCTCSSRX::process(const q15_t* samples, uint8_t length)
{
  for (unsigned int i = 0U; i < length; i++) {

    long sampleLong = (long)samples[i];

    long q2 = m_q1;
    m_q1 = m_q0;
    m_q0 = m_coeff * m_q1 - q2 + sampleLong;

    m_count++;
    if(m_count == N) {
      m_count = 0;
      if(!m_result) {
        long magnitudeSquared = m_q0 * m_q0 + m_q1 * m_q1 - m_q0 * m_q1 * m_coeff;
        if(magnitudeSquared >= m_thresholdSquared) {
          m_result = magnitudeSquared >= m_thresholdSquared;
          m_q0 = 0;
          m_q1 = 0;
        }
      }
    }
  }

  return m_result;
}

void CFMCTCSSRX::reset()
{
  m_q0 = 0;
  m_q1 = 0;
  m_result = false;
  m_count  = 0U;
}
