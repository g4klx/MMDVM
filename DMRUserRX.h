/*
 *   Copyright (C) 2015,2020 by Jonathan Naylor G4KLX
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

#if defined(MODE_DMR)

#if !defined(DMRUSERRX_H)
#define  DMRUSERRX_H

#include "DMRDefines.h"

const unsigned int DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES = DMR_FRAME_LENGTH_SAMPLES + DMR_CACH_LENGTH_SAMPLES;

class CDMRUserRX {
public:
  CDMRUserRX();

  void samples(const q15_t* samples, uint8_t length);

  void setColorCode(uint8_t colorCode);

  void reset();

private:
  uint32_t m_bitBuffer[DMR_RADIO_SYMBOL_LENGTH];
  q15_t    m_buffer[DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES];
  uint16_t m_bitPtr;
  uint16_t m_dataPtr;
  uint16_t m_endPtr;
  q31_t    m_maxCorr;
  q15_t    m_centre;
  q15_t    m_threshold;
  uint8_t  m_colorCode;
  bool     m_slot;

  bool cachCheck(uint8_t* cach);
  void extractBits(uint8_t* bytes, uint16_t start, uint8_t count_bits, bool* buffer);

  void processSample(q15_t sample);
  void samplesToBits(uint16_t start, uint8_t count, uint8_t* buffer, uint16_t offset, q15_t centre, q15_t threshold);
};

#endif

#endif

