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
m_ringBuffer(length),//length might need tweaking
m_packIndex(0),
m_downSampleIndex(0)
{
    m_samplePack = 0;
}

void CFMDownsampler::addSample(q15_t sample)
{
    //only take one of three samples
    if(m_downSampleIndex == 0) {
      switch(m_packIndex){
        case 0:
          m_samplePack = int32_t(sample) << 12;
        break;
        case 1:{
            m_samplePack |= int32_t(sample);
            
            //we did not use MSB; skip it
            m_ringBuffer.put(m_samplePackBytes[1]); 
            m_ringBuffer.put(m_samplePackBytes[2]); 
            m_ringBuffer.put(m_samplePackBytes[3]); 

            m_samplePack = 0;
        }
        break;
        default:
            //should never happen
        break;
        }
        m_packIndex++;
        if(m_packIndex >= 2)
            m_packIndex = 0;  
    }

    m_downSampleIndex++;
    if(m_downSampleIndex >= 3)
        m_downSampleIndex = 0;
}