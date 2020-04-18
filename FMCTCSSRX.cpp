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
  uint8_t   frequency;
  float32_t coeff;
} CTCSS_TABLE_DATA[] = {
  { 67U, 1.999692},
  { 69U, 1.999671},
  { 71U, 1.999646},
  { 74U, 1.999621},
  { 77U, 1.999594},
  { 79U, 1.999565},
  { 82U, 1.999534},
  { 85U, 1.999500},
  { 88U, 1.999463},
  { 91U, 1.999426},
  { 94U, 1.999384},
  { 97U, 1.999350},
  {100U, 1.999315},
  {103U, 1.999266},
  {107U, 1.999212},
  {110U, 1.999157},
  {114U, 1.999097},
  {118U, 1.999033},
  {123U, 1.998963},
  {127U, 1.998889},
  {131U, 1.998810},
  {136U, 1.998723},
  {141U, 1.998632},
  {146U, 1.998535},
  {151U, 1.998429},
  {156U, 1.998317},
  {159U, 1.998250},
  {162U, 1.998197},
  {165U, 1.998123},
  {167U, 1.998068},
  {171U, 1.997989},
  {173U, 1.997930},
  {177U, 1.997846},
  {179U, 1.997782},
  {183U, 1.997693},
  {186U, 1.997624},
  {189U, 1.997529},
  {192U, 1.997453},
  {196U, 1.997351},
  {199U, 1.997273},
  {203U, 1.997162},
  {206U, 1.997078},
  {210U, 1.996958},
  {218U, 1.996741},
  {225U, 1.996510},
  {229U, 1.996404},
  {233U, 1.996261},
  {241U, 1.995994},
  {250U, 1.995708},
  {254U, 1.995576}};

const uint8_t CTCSS_TABLE_DATA_LEN = 50U;

// 4Hz bandwidth
const uint16_t N = 24000U / 4U;

CFMCTCSSRX::CFMCTCSSRX() :
m_coeff(0.0),
m_threshold(0.0),
m_count(0U),
m_q0(0.0),
m_q1(0.0),
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

  if (m_coeff == 0.0)
    return 4U;

  m_threshold = float32_t(threshold * threshold);

  return 0U;
}

bool CFMCTCSSRX::process(q15_t* samples, uint8_t length)
{
  float32_t data[RX_BLOCK_SIZE];
  ::arm_q15_to_float(samples, data, length);

  for (unsigned int i = 0U; i < length; i++) {
    float32_t q2 = m_q1;
    m_q1 = m_q0;
    m_q0 = m_coeff * m_q1 - q2 + data[i];

    m_count++;
    if (m_count == N) {
      float32_t value = m_q0 * m_q0 + m_q1 * m_q1 - m_q0 * m_q1 * m_coeff;
      m_result = value >= m_threshold;
      m_count = 0U;
      m_q0 = 0.0F;
      m_q1 = 0.0F;
    }
  }

  return m_result;
}

void CFMCTCSSRX::reset()
{
  m_q0 = 0.0;
  m_q1 = 0.0;
  m_result = false;
  m_count  = 0U;
}
