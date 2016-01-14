/*
 *   Copyright (C) 2015 by Jonathan Naylor G4KLX
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

  void samples(const q15_t* samples, uint8_t length);

  void reset();

private:
  uint32_t    m_pll;
  bool        m_prev;
  YSFRX_STATE m_state;
  uint32_t    m_symbolBuffer;
  uint64_t    m_bitBuffer;
  q15_t       m_symbols[YSF_SYNC_LENGTH_SYMBOLS];
  uint8_t     m_outBuffer[YSF_FRAME_LENGTH_BYTES + 1U];
  uint8_t*    m_buffer;
  uint16_t    m_bufferPtr;
  uint16_t    m_symbolPtr;
  uint16_t    m_lostCount;
  q15_t       m_centre;
  q15_t       m_threshold;
  uint16_t    m_metrics1[16U];
  uint16_t    m_metrics2[16U];
  uint16_t*   m_oldMetrics;
  uint16_t*   m_newMetrics;
  uint64_t    m_decisions[100U];
  uint64_t*   m_dp;

  void processNone(q15_t sample);
  void processData(q15_t sample);
  bool rxFICH(uint8_t* data, uint8_t* FICH);
  void viterbiDecode(uint8_t s0, uint8_t s1);
  void chainback(uint8_t* out);
  uint32_t getSyndrome23127(uint32_t pattern) const;
  uint32_t golay24128(const uint8_t* bytes) const;
  bool checksum(const uint8_t* fich) const;
};

#endif

