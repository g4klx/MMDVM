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
  { 67U, 1.999692F},
  { 69U, 1.999671F},
  { 71U, 1.999646F},
  { 74U, 1.999621F},
  { 77U, 1.999594F},
  { 79U, 1.999565F},
  { 82U, 1.999534F},
  { 85U, 1.999500F},
  { 88U, 1.999463F},
  { 91U, 1.999426F},
  { 94U, 1.999384F},
  { 97U, 1.999350F},
  {100U, 1.999315F},
  {103U, 1.999266F},
  {107U, 1.999212F},
  {110U, 1.999157F},
  {114U, 1.999097F},
  {118U, 1.999033F},
  {123U, 1.998963F},
  {127U, 1.998889F},
  {131U, 1.998810F},
  {136U, 1.998723F},
  {141U, 1.998632F},
  {146U, 1.998535F},
  {151U, 1.998429F},
  {156U, 1.998317F},
  {159U, 1.998250F},
  {162U, 1.998197F},
  {165U, 1.998123F},
  {167U, 1.998068F},
  {171U, 1.997989F},
  {173U, 1.997930F},
  {177U, 1.997846F},
  {179U, 1.997782F},
  {183U, 1.997693F},
  {186U, 1.997624F},
  {189U, 1.997529F},
  {192U, 1.997453F},
  {196U, 1.997351F},
  {199U, 1.997273F},
  {203U, 1.997162F},
  {206U, 1.997078F},
  {210U, 1.996958F},
  {218U, 1.996741F},
  {225U, 1.996510F},
  {229U, 1.996404F},
  {233U, 1.996261F},
  {241U, 1.995994F},
  {250U, 1.995708F},
  {254U, 1.995576F}};

const uint8_t CTCSS_TABLE_DATA_LEN = 50U;

// 4Hz bandwidth
const uint16_t N = 24000U / 4U;

CFMCTCSSRX::CFMCTCSSRX() :
m_coeff(0.0F),
m_threshold(0.0F),
m_count(0U),
m_q0(0.0F),
m_q1(0.0F),
m_result((CTCSSState)0U)
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

  if (m_coeff == 0.0F)
    return 4U;

  m_threshold = float32_t(threshold);

  return 0U;
}

CTCSSState CFMCTCSSRX::process(q15_t sample)
{
  m_result = m_result & (~CTS_READY);
  float32_t q2 = m_q1;
  m_q1 = m_q0;
  m_q0 = m_coeff * m_q1 - q2 + float32_t(sample) / 32768.0F;

  m_count++;
  if (m_count == N) {
    float32_t value = m_q0 * m_q0 + m_q1 * m_q1 - m_q0 * m_q1 * m_coeff;
    m_result = m_result | CTS_READY;
    if(value >= m_threshold)
      m_result = m_result | CTS_VALID;
    else
      m_result = m_result & ~CTS_VALID;

    m_count = 0U;
    m_q0 = 0.0F;
    m_q1 = 0.0F;
  }

  return m_result;
}

CTCSSState CFMCTCSSRX::getState()
{
  return m_result;
}

void CFMCTCSSRX::reset()
{
  m_q0 = 0.0F;
  m_q1 = 0.0F;
  m_result = (CTCSSState)0U;
  m_count  = 0U;
}
