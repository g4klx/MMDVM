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

#if !defined(DSTARRX_H)
#define  DSTARRX_H

#include "Config.h"
#include "DStarDefines.h"

enum DSRX_STATE {
  DSRXS_NONE,
  DSRXS_HEADER,
  DSRXS_DATA
};

class CDStarRX {
public:
  CDStarRX();

  void samples(const q15_t* samples, const uint16_t* rssi, uint8_t length);

  void reset();

private:
  uint32_t     m_pll;
  bool         m_prev;
  DSRX_STATE   m_rxState;
  uint32_t     m_patternBuffer;
  uint8_t      m_rxBuffer[100U];
  unsigned int m_rxBufferBits;
  unsigned int m_dataBits;
  unsigned int m_mar;
  int          m_pathMetric[4U];
  unsigned int m_pathMemory0[42U];
  unsigned int m_pathMemory1[42U];
  unsigned int m_pathMemory2[42U];
  unsigned int m_pathMemory3[42U];
  uint8_t      m_fecOutput[42U];
  uint32_t     m_rssiAccum;
  uint16_t     m_rssiCount;
  
  void    processNone(bool bit);
  void    processHeader(bool bit);
  void    processData(bool bit);
  void    writeRSSIHeader(unsigned char* header);
  void    writeRSSIData(unsigned char* data);
  bool    rxHeader(uint8_t* in, uint8_t* out);
  void    acs(int* metric);
  void    viterbiDecode(int* data);
  void    traceBack();
  bool    checksum(const uint8_t* header) const;
};

#endif

