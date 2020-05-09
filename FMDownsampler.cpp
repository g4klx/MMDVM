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
#include "FMDownsampler.h"


CFMDownsampler::CFMDownsampler(uint16_t length) :
m_ringBuffer(length),
m_samplePack(0U),
m_samplePackPointer(NULL),
m_packIndex(0U),
m_downSampleIndex(0U)
{
    m_samplePackPointer = &m_samplePack;
}

void CFMDownsampler::addSample(q15_t sample)
{
    //only take one of three samples
    if(m_downSampleIndex == 0U) {
      switch(m_packIndex){
        case 0:
          m_samplePack = uint32_t(sample) << 12;
        break;
        case 1:{
            m_samplePack |= uint32_t(sample);
            
            //we did not use MSB; skip it
            m_ringBuffer.put(m_samplePackPointer[1U]); 
            m_ringBuffer.put(m_samplePackPointer[2U]); 
            m_ringBuffer.put(m_samplePackPointer[3U]); 

            m_samplePack = 0;//reset the sample pack
        }
        break;
        default:
            //should never happen
        break;
        }
        m_packIndex++;
        if(m_packIndex >= 2U)//did we pack to samples ?
            m_packIndex = 0U;  
    }

    m_downSampleIndex++;
    if(m_downSampleIndex >= 3U)
        m_downSampleIndex = 0U;
}

bool CFMDownsampler::getPackedData(uint8_t& data)
{
  return m_ringBuffer.get(data);
}

void CFMDownsampler::reset()
{
  m_downSampleIndex = 0;
  m_packIndex = 0;
}