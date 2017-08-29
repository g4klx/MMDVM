/*
 *   Copyright (C) 2015,2016,2017 by Jonathan Naylor G4KLX
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

#if !defined(YSFRX_H)
#define  YSFRX_H

#include "Config.h"
#include "YSFDefines.h"

enum YSFRX_STATE {
  YSFRXS_NONE,
  YSFRXS_DATA
};

class CYSFRX {
public:
  CYSFRX();

  void samples(const q15_t* samples, uint16_t* rssi, uint8_t length);

  void reset();

private:
  YSFRX_STATE m_state;
  uint32_t    m_bitBuffer[YSF_RADIO_SYMBOL_LENGTH];
  q15_t       m_buffer[YSF_FRAME_LENGTH_SAMPLES];
  uint16_t    m_bitPtr;
  uint16_t    m_dataPtr;
  uint16_t    m_startPtr;
  uint16_t    m_endPtr;
  uint16_t    m_syncPtr;
  uint16_t    m_minSyncPtr;
  uint16_t    m_maxSyncPtr;
  q31_t       m_maxCorr;
  uint16_t    m_lostCount;
  uint8_t     m_countdown;
  q15_t       m_centre[16U];
  q15_t       m_centreVal;
  q15_t       m_threshold[16U];
  q15_t       m_thresholdVal;
  uint8_t     m_averagePtr;
  uint32_t    m_rssiAccum;
  uint16_t    m_rssiCount;

  void processNone(q15_t sample);
  void processData(q15_t sample);
  bool correlateSync();
  void calculateLevels(uint16_t start, uint16_t count);
  void samplesToBits(uint16_t start, uint16_t count, uint8_t* buffer, uint16_t offset, q15_t centre, q15_t threshold);
  void writeRSSIData(uint8_t* data);
};

#endif

