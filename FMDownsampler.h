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

#if !defined(FMDOWNSAMPLER_H)
#define FMDOWNSAMPLER_H

#include "Config.h"
#include "FMDownsampleRB.h"

class CFMDownsampler {
public:
  CFMDownsampler(uint16_t length);
  void addSample(q15_t sample);
  inline bool getPackedData(uint8_t& data){ return m_ringBuffer.get(data); };
  inline bool hasOverflowed() { return m_ringBuffer.hasOverflowed(); };

private:
  CFMDownsampleRB m_ringBuffer;
  union {
    int32_t m_samplePack;
    int8_t  m_samplePackBytes[4];
  };
  uint8_t m_packIndex;
  uint8_t m_downSampleIndex;
};

#endif
