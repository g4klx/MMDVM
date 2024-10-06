/*
 *   Copyright (C) 2015,2016,2017,2020,2021 by Jonathan Naylor G4KLX
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

#if defined(MODE_M17)

#if !defined(M17RX_H)
#define  M17RX_H

#include "M17Defines.h"

enum M17RX_STATE {
  M17RXS_NONE,
  M17RXS_LINK_SETUP,
  M17RXS_STREAM
};

class CM17RX {
public:
  CM17RX();

  void samples(const q15_t* samples, uint16_t* rssi, uint8_t length);

  void reset();

private:
  M17RX_STATE m_state;
  uint8_t     m_bitBuffer[M17_RADIO_SYMBOL_LENGTH];
  q15_t       m_buffer[M17_FRAME_LENGTH_SAMPLES];
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
  M17RX_STATE m_nextState;
  q15_t       m_centre[16U];
  q15_t       m_centreVal;
  q15_t       m_threshold[16U];
  q15_t       m_thresholdVal;
  uint8_t     m_averagePtr;
  uint32_t    m_rssiAccum;
  uint16_t    m_rssiCount;

  void processNone(q15_t sample);
  void processData(q15_t sample);
  bool correlateSync(uint8_t syncSymbols, const int8_t* syncSymbolValues, const uint8_t* syncBytes, uint8_t maxSymbolErrs, uint8_t maxBitErrs);
  void calculateLevels(uint16_t start, uint16_t count);
  void samplesToBits(uint16_t start, uint16_t count, uint8_t* buffer, uint16_t offset, q15_t centre, q15_t threshold);
  void writeRSSILinkSetup(uint8_t* data);
  void writeRSSIStream(uint8_t* data);
};

#endif

#endif

