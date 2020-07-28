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
#include "FMNoiseSquelch.h"

// 4500Hz centre frequency
const q31_t COEFF_DIV_TWO = 821806413;

// 400Hz bandwidth
const uint16_t N = 24000U / 400U;

CFMNoiseSquelch::CFMNoiseSquelch() :
m_highThreshold(0),
m_lowThreshold(0),
m_count(0U),
m_q0(0),
m_q1(0),
m_result(NS_NONE)
{
}

void CFMNoiseSquelch::setParams(uint8_t highThreshold, uint8_t lowThreshold)
{
  m_highThreshold = q31_t(highThreshold);
  m_lowThreshold  = q31_t(lowThreshold);
}

uint8_t CFMNoiseSquelch::process(q15_t sample)
{
  //get more dynamic into the decoder by multiplying the sample by 1.5
  q31_t sample31 = q31_t(sample) +  (q31_t(sample) >> 1);

  m_result &= ~NS_READY;

  q31_t q2 = m_q1;
  m_q1 = m_q0;

  // Q31 multiplication, t3 = m_coeffDivTwo * 2 * m_q1
  q63_t t1 = COEFF_DIV_TWO * m_q1;
  q31_t t2 = __SSAT((t1 >> 31), 31);
  q31_t t3 = t2 * 2;

  // m_q0 = m_coeffDivTwo * m_q1 * 2 - q2 + sample
  m_q0 = t3 - q2 + sample31;

  m_count++;
  if (m_count == N) {
    // Q31 multiplication, t2 = m_q0 * m_q0
    q63_t t1 = q63_t(m_q0) * q63_t(m_q0);
    q31_t t2 = __SSAT((t1 >> 31), 31);

    // Q31 multiplication, t4 = m_q0 * m_q0
    q63_t t3 = q63_t(m_q1) * q63_t(m_q1);
    q31_t t4 = __SSAT((t3 >> 31), 31);

    // Q31 multiplication, t9 = m_q0 * m_q1 * m_coeffDivTwo * 2
    q63_t t5 = q63_t(m_q0) * q63_t(m_q1);
    q31_t t6 = __SSAT((t5 >> 31), 31);
    q63_t t7 = t6 * COEFF_DIV_TWO;
    q31_t t8 = __SSAT((t7 >> 31), 31);
    q31_t t9  = t8 * 2;

    // value = m_q0 * m_q0 + m_q1 * m_q1 - m_q0 * m_q1 * m_coeffDivTwo * 2
    q31_t value = t2 + t4 - t9;

    bool previousNSQValid = NSQ_VALID(m_result);

    q31_t threshold = m_highThreshold;
    if (previousNSQValid)
      threshold = m_lowThreshold;

    m_result |= NS_READY;
    if (value < threshold)
      m_result |= NS_VALID;
    else
      m_result &= ~NS_VALID;

    if (previousNSQValid != NSQ_VALID(m_result))
      DEBUG4("Noise Squelch Value / Threshold / Valid", value, threshold, NSQ_VALID(m_result));

    m_count = 0U;
    m_q0 = 0;
    m_q1 = 0;
  }

  return m_result;
}

void CFMNoiseSquelch::reset()
{
  m_q0 = 0;
  m_q1 = 0;
  m_result = NS_NONE;
  m_count  = 0U;
}
