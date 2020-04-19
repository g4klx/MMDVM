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
  q31_t   coeff;
} CTCSS_TABLE_DATA[] = {
  { 67U, 131052},
  { 69U, 131051},
  { 71U, 131049},
  { 74U, 131048},
  { 77U, 131046},
  { 79U, 131044},
  { 82U, 131042},
  { 85U, 131040},
  { 88U, 131037},
  { 91U, 131035},
  { 94U, 131032},
  { 97U, 131030},
  {100U, 131028},
  {103U, 131024},
  {107U, 131021},
  {110U, 131017},
  {114U, 131013},
  {118U, 131009},
  {123U, 131005},
  {127U, 131000},
  {131U, 130994},
  {136U, 130989},
  {141U, 130983},
  {146U, 130977},
  {151U, 130970},
  {156U, 130962},
  {159U, 130958},
  {162U, 130954},
  {165U, 130949},
  {167U, 130946},
  {171U, 130941},
  {173U, 130937},
  {177U, 130931},
  {179U, 130927},
  {183U, 130921},
  {186U, 130917},
  {189U, 130911},
  {192U, 130906},
  {196U, 130899},
  {199U, 130894},
  {203U, 130887},
  {206U, 130881},
  {210U, 130873},
  {218U, 130859},
  {225U, 130844},
  {229U, 130837},
  {233U, 130827},
  {241U, 130810},
  {250U, 130791},
  {254U, 130783}};

const uint8_t CTCSS_TABLE_DATA_LEN = 50U;

// 4Hz bandwidth
const uint16_t N = 24000U / 4U;

CFMCTCSSRX::CFMCTCSSRX() :
m_coeff(0),
m_threshold(0U),
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

  m_threshold = threshold * threshold;

  return 0U;
}

bool CFMCTCSSRX::process(const q15_t* samples, uint8_t length)
{
  for (unsigned int i = 0U; i < length; i++) {
    q31_t q2 = m_q1;
    m_q1 = m_q0;

    // Q31 multiplication, t2 = m_coeff * m_q1
    q63_t t1 = m_coeff * m_q1;
    q31_t t2 = __SSAT(t1 >> 32, 31);

    // m_q0 = m_coeff * m_q1 - q2 + samples[i]
    m_q0 = t2 - q2 + samples[i];

    m_count++;
    if (m_count == N) {
      // Q31 multiplication, t2 = m_q0 * m_q0
      q63_t t1 = m_q0 * m_q0;
      q31_t t2 = __SSAT(t1 >> 32, 31);

      // Q31 multiplication, t4 = m_q0 * m_q0
      q63_t t3 = m_q1 * m_q1;
      q31_t t4 = __SSAT(t3 >> 32, 31);

      // Q31 multiplication, t8 = m_q0 * m_q1 * m_coeff
      q63_t t5 = m_q0 * m_q1;
      q31_t t6 = __SSAT(t5 >> 32, 31);
      q63_t t7 = t6 * m_coeff;
      q31_t t8 = __SSAT(t7 >> 32, 31);

      // value = m_q0 * m_q0 + m_q1 * m_q1 - m_q0 * m_q1 * m_coeff
      q31_t value = t2 + t4 - t8;

      m_result = value >= m_threshold;
      m_count = 0U;
      m_q0 = 0;
      m_q1 = 0;
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
