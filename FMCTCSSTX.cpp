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
#include "FMCTCSSTX.h"

const struct CTCSS_TABLE {
  uint8_t  frequency;
  uint16_t length;
  q15_t    increment;
} CTCSS_TABLE_DATA[] = {
  { 67U, 358U,  92},
  { 69U, 346U,  95},
  { 71U, 334U,  99},
  { 74U, 323U, 102},
  { 77U, 312U, 106},
  { 79U, 301U, 109},
  { 82U, 291U, 113},
  { 85U, 281U, 117},
  { 88U, 271U, 121},
  { 91U, 262U, 125},
  { 94U, 253U, 130},
  { 97U, 246U, 133},
  {100U, 240U, 137},
  {103U, 232U, 142},
  {107U, 224U, 147},
  {110U, 216U, 152},
  {114U, 209U, 157},
  {118U, 202U, 163},
  {123U, 195U, 168},
  {127U, 189U, 174},
  {131U, 182U, 180},
  {136U, 176U, 187},
  {141U, 170U, 193},
  {146U, 164U, 200},
  {151U, 159U, 207},
  {156U, 153U, 214},
  {159U, 150U, 219},
  {162U, 148U, 222},
  {165U, 145U, 226},
  {167U, 143U, 230},
  {171U, 140U, 234},
  {173U, 138U, 238},
  {177U, 135U, 243},
  {179U, 133U, 246},
  {183U, 131U, 251},
  {186U, 129U, 255},
  {189U, 126U, 260},
  {192U, 124U, 264},
  {196U, 122U, 269},
  {199U, 120U, 273},
  {203U, 118U, 278},
  {206U, 116U, 282},
  {210U, 114U, 288},
  {218U, 110U, 298},
  {225U, 106U, 309},
  {229U, 105U, 313},
  {233U, 103U, 319},
  {241U,  99U, 331},
  {250U,  96U, 342},
  {254U,  94U, 347}
};

const uint8_t CTCSS_TABLE_DATA_LEN = 50U;

CFMCTCSSTX::CFMCTCSSTX() :
m_values(NULL),
m_length(0U),
m_n(0U)
{
}

uint8_t CFMCTCSSTX::setParams(uint8_t frequency, uint8_t level)
{
  const CTCSS_TABLE* entry = NULL;

  for (uint8_t i = 0U; i < CTCSS_TABLE_DATA_LEN; i++) {
    if (CTCSS_TABLE_DATA[i].frequency == frequency) {
      entry = CTCSS_TABLE_DATA + i;
      break;
    }
  }

  if (entry == NULL)
    return 4U;

  m_length = entry->length;

  m_values = new q15_t[m_length];

  q15_t arg = 0;
  for (uint16_t i = 0U; i < m_length; i++) {
    q31_t value = ::arm_sin_q15(arg) * q15_t(level * 128);
    m_values[i] = q15_t(__SSAT((value >> 15), 16));

    arg += entry->increment;
  }

  return 0U;
}

void CFMCTCSSTX::getAudio(q15_t* samples, uint8_t length)
{
  for (uint8_t i = 0U; i < length; i++) {
    samples[i] += m_values[m_n];

    m_n++;
    if (m_n >= m_length)
      m_n = 0U;
  }
}
