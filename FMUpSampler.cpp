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

#include "Config.h"

#if defined(MODE_FM)

#include "FMUpSampler.h"

const uint32_t FM_UPSAMPLE_MASK = 0x00000FFFU;

CFMUpSampler::CFMUpSampler() :
m_upSampleIndex(0),
m_pack(0U),
m_packPointer(NULL),
m_samples(3600U), //300ms of 12 bit 8kHz audio
m_running(false)
{
  m_packPointer = (uint8_t*)&m_pack;
}

void CFMUpSampler::reset()
{
    m_upSampleIndex = 0U;
    m_pack = 0U;
    m_samples.reset();
    m_running = false;
}

void CFMUpSampler::addData(const uint8_t* data, uint16_t length)
{
  TSamplePairPack* packPointer = (TSamplePairPack*)data;
  TSamplePairPack* packPointerEnd = packPointer + (length / 3U);
  while(packPointer != packPointerEnd) {
    m_samples.put(*packPointer);
    packPointer++;
  }
  if(!m_running)
    m_running = m_samples.getData() > 300U;//75ms of audio
}

bool CFMUpSampler::getSample(q15_t& sample)
{
  if(!m_running)
    return false;

  switch (m_upSampleIndex)
  {
  case 0: {
    TSamplePairPack pairPack;
    if(!m_samples.get(pairPack)) {
      m_running = false;
      return false;
    }
      
    m_pack = 0U;
    m_packPointer = (uint8_t*)&m_pack;

    m_packPointer[0U] = pairPack.byte0;
    m_packPointer[1U] = pairPack.byte1;
    m_packPointer[2U] = pairPack.byte2;

    sample = q15_t(m_pack >> 12) - 2048;
    break;
  }
  case 3:
    sample = q15_t(m_pack & FM_UPSAMPLE_MASK) - 2048;
  break;
  default:
    sample = 0;
  break;
  }

  m_upSampleIndex++;
  if(m_upSampleIndex >= 6U)
    m_upSampleIndex = 0U;

  return true;
}

uint16_t CFMUpSampler::getSpace() const
{
  //return available space in bytes
  return m_samples.getSpace() * sizeof(TSamplePairPack);
}

#endif

