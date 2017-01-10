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

  void samples(const q15_t* samples, const uint16_t* rssi, uint8_t length);

  void reset();

private:
  uint32_t    m_pll;
  bool        m_prev;
  YSFRX_STATE m_state;
  uint32_t    m_symbolBuffer;
  uint64_t    m_bitBuffer;
  q15_t       m_symbols[YSF_SYNC_LENGTH_SYMBOLS];
  uint8_t     m_outBuffer[YSF_FRAME_LENGTH_BYTES + 3U];
  uint8_t*    m_buffer;
  uint16_t    m_bufferPtr;
  uint16_t    m_symbolPtr;
  uint16_t    m_lostCount;
  q15_t       m_centre;
  q15_t       m_threshold;
  uint32_t    m_rssiAccum;
  uint16_t    m_rssiCount;

  void processNone(q15_t sample);
  void processData(q15_t sample);
  void writeRSSIData(unsigned char* data);
};

#endif

