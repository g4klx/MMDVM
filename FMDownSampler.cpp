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

#include "FMDownSampler.h"

CFMDownSampler::CFMDownSampler(uint16_t length) :
m_ringBuffer(length),
m_samplePack(0U),
m_samplePackPointer(NULL),
m_sampleIndex(0U)
{
  m_samplePackPointer = (uint8_t*)&m_samplePack;
}

void CFMDownSampler::addSample(q15_t sample)
{
  uint32_t usample = uint32_t(int32_t(sample) + 2048);
  //only take one of three samples
  switch(m_sampleIndex){
    case 0:
      m_samplePack = usample << 12;
    break;
    case 3:{
      m_samplePack |= usample;
      
      //we did not use MSB; skip it
      TSamplePairPack pair{m_samplePackPointer[0U], m_samplePackPointer[1U], m_samplePackPointer[2U]}; 

      m_ringBuffer.put(pair);

      m_samplePack = 0U;//reset the sample pack
    }
    break;
    default:
        //Just skip this sample
    break;
  }

  m_sampleIndex++;
  if (m_sampleIndex >= 6U)//did we pack two samples ?
      m_sampleIndex = 0U;  
}

bool CFMDownSampler::getPackedData(TSamplePairPack& data)
{
  return m_ringBuffer.get(data);
}

uint16_t CFMDownSampler::getData()
{
  return m_ringBuffer.getData();
}

void CFMDownSampler::reset()
{
  m_sampleIndex = 0U;
}

#endif

