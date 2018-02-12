/*
 *   Copyright (C) 2009-2015 by Jonathan Naylor G4KLX
 *   Copyright (C) 2016 by Colin Durbridge G4EML
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

#if !defined(CALDMR_H)
#define  CALDMR_H

#include "Config.h"
#include "DMRDefines.h"

enum DMRCAL1K {
  DMRCAL1K_IDLE,
  DMRCAL1K_VH,
  DMRCAL1K_VOICE,
  DMRCAL1K_VT,
  DMRCAL1K_WAIT
};

class CCalDMR {
public:
  CCalDMR();

  void process();
  void dmr1kcal();
  void dmrdmo1k();
  void createData1k(uint8_t n);
  void createDataDMO1k(uint8_t n);

  uint8_t write(const uint8_t* data, uint8_t length);

private:
  bool      m_transmit;
  DMRCAL1K  m_state;
  uint32_t  m_frame_start;
  uint8_t   m_dmr1k[DMR_FRAME_LENGTH_BYTES + 1U];
  uint8_t   m_audioSeq;
};

#endif

