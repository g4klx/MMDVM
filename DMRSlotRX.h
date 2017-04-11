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

#if !defined(DMRSLOTRX_H)
#define  DMRSLOTRX_H

#include "Config.h"
#include "DMRDefines.h"

enum DMRRX_STATE {
  DMRRXS_NONE,
  DMRRXS_VOICE,
  DMRRXS_DATA
};

class CDMRSlotRX {
public:
  CDMRSlotRX(bool slot);

  void start();

  bool processSample(q15_t sample, uint16_t rssi);

  void setColorCode(uint8_t colorCode);
  void setDelay(uint8_t delay);

  void reset();

private:
  bool        m_slot;
  uint32_t    m_bitBuffer[DMR_RADIO_SYMBOL_LENGTH];
  q15_t       m_buffer[900U];
  uint16_t    m_bitPtr;
  uint16_t    m_dataPtr;
  uint16_t    m_syncPtr;
  uint16_t    m_startPtr;
  uint16_t    m_endPtr;
  uint16_t    m_delayPtr;
  q31_t       m_maxCorr;
  q15_t       m_centre[4U];
  q15_t       m_threshold[4U];
  uint8_t     m_averagePtr;
  uint8_t     m_control;
  uint8_t     m_syncCount;
  uint8_t     m_colorCode;
  uint16_t    m_delay;
  DMRRX_STATE m_state;
  uint8_t     m_n;
  uint8_t     m_type;
  uint16_t    m_rssi[900U];

  void correlateSync(bool first);
  void samplesToBits(uint16_t start, uint8_t count, uint8_t* buffer, uint16_t offset, q15_t centre, q15_t threshold);
  void writeRSSIData(uint8_t* frame);
};

#endif
