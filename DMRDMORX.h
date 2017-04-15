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

#if !defined(DMRDMORX_H)
#define  DMRDMORX_H

#include "Config.h"
#include "DMRDefines.h"

const uint16_t DMO_BUFFER_LENGTH_SAMPLES = 1440U;   // 60ms at 24 kHz

enum DMORX_STATE {
  DMORXS_NONE,
  DMORXS_VOICE,
  DMORXS_DATA
};

class CDMRDMORX {
public:
  CDMRDMORX();

  void samples(const q15_t* samples, const uint16_t* rssi, uint8_t length);

  void setColorCode(uint8_t colorCode);

  void reset();

private:
  uint32_t    m_bitBuffer[DMR_RADIO_SYMBOL_LENGTH];
  q15_t       m_buffer[DMO_BUFFER_LENGTH_SAMPLES];
  uint16_t    m_bitPtr;
  uint16_t    m_dataPtr;
  uint16_t    m_syncPtr;
  uint16_t    m_startPtr;
  uint16_t    m_endPtr;
  q31_t       m_maxCorr;
  q15_t       m_centre[4U];
  q15_t       m_threshold[4U];
  uint8_t     m_averagePtr;
  uint8_t     m_control;
  uint8_t     m_syncCount;
  uint8_t     m_colorCode;
  DMORX_STATE m_state;
  uint8_t     m_n;
  uint8_t     m_type;
  uint16_t    m_rssi[DMO_BUFFER_LENGTH_SAMPLES];
  
  bool processSample(q15_t sample, uint16_t rssi);
  void correlateSync(bool first);
  void samplesToBits(uint16_t start, uint8_t count, uint8_t* buffer, uint16_t offset, q15_t centre, q15_t threshold);
  void writeRSSIData(uint8_t* frame);
};

#endif
