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


#if !defined(FMUPSAMPLER_H)
#define FMUPSAMPLER_H

#include "Config.h"
#include "RingBuffer.h"
#include "FMSamplePairPack.h"

class CFMUpSampler {
public:
  CFMUpSampler(uint16_t length);

  void reset();

  void addData(const uint8_t* data, uint16_t length);

  bool getSample(q15_t& sample);

  uint16_t getSpace() const;

private:
  uint8_t m_upSampleIndex;
  TSamplePairPack m_pack;
  CRingBuffer<q15_t> m_samples;
};

#endif