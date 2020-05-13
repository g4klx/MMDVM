/*
 *   Copyright (C) 2020 by Jonathan Naylor G4KLX
 *   Copyright (C) 2020 by Geoffrey Merck F4FXL - KC3FRA
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

#include "FMUpSampler.h"

const uint32_t FM_UPSAMPLE_MASK = 0x00000FFFU;

CFMUpSampler::CFMUpSampler(uint16_t length) :
m_upSampleIndex(0),
m_pack({0U, 0U, 0U}),
m_samples(length)
{
}

void CFMUpSampler::reset()
{
    m_upSampleIndex = 0U;
    m_pack = {0U, 0U, 0U};
    m_samples.reset();
}

void CFMUpSampler::addData(const uint8_t* data, uint16_t length)
{
  for(uint16_t i = 0U; i < length; i++) {
    switch (m_upSampleIndex)
    {
      case 0U:
        m_pack.byte0 = data[i];
      break;
      case 1U:
        m_pack.byte1 = data[i];
      break;
      case 2U: {
        m_pack.byte2 = data[i];

        uint32_t pack = 0U;
        uint8_t* packPtr = (uint8_t*)&pack;

        packPtr[1] = m_pack.byte0;
        packPtr[2] = m_pack.byte1;
        packPtr[3] = m_pack.byte2;

        q15_t sample2 = q15_t(uint16_t(pack & FM_UPSAMPLE_MASK) - 2048);
        q15_t sample1 = q15_t(uint16_t(pack >> 12) - 2048);

        m_samples.put(sample1);
        m_samples.put(0);
        m_samples.put(0);
        m_samples.put(sample2);
        m_samples.put(0);
        m_samples.put(0);
      }
      break;
      default:
      //shoudl never happen
      break;
    }

    m_upSampleIndex ++;
    if (m_upSampleIndex > 2U)
      m_upSampleIndex = 0U;
  }
}

bool CFMUpSampler::getSample(q15_t& sample)
{
    return m_samples.get(sample);
}

uint16_t CFMUpSampler::getSpace() const
{
    return m_samples.getSpace();
}