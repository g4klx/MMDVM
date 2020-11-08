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

#if !defined(FMDOWNSAMPLER_H)
#define FMDOWNSAMPLER_H

#include "RingBuffer.h"
#include "FMSamplePairPack.h"

class CFMDownSampler {
public:
  CFMDownSampler(uint16_t length);

  void addSample(q15_t sample);

  bool getPackedData(TSamplePairPack& data);

  uint16_t getData();

  void reset();

private:
  CRingBuffer<TSamplePairPack> m_ringBuffer;
  uint32_t                     m_samplePack;
  uint8_t*                     m_samplePackPointer;
  uint8_t                      m_sampleIndex;
};

#endif

#endif

