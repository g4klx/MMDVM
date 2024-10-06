/*
 *   Copyright (C) 2020 by Jonathan Naylor G4KLX
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

#if defined(MODE_AX25)

#if !defined(AX25RX_H)
#define  AX25RX_H

#include "AX25Demodulator.h"

class CAX25RX {
public:
  CAX25RX();

  void samples(q15_t* samples, uint8_t length);

  void setParams(int8_t twist, uint8_t slotTime, uint8_t pPersist);

  bool canTX() const;

private:
  arm_fir_instance_q15 m_filter;
  q15_t                m_state[160U];    // NoTaps + BlockSize - 1, 130 + 20 - 1 plus some spare
  CAX25Demodulator     m_demod1;
  CAX25Demodulator     m_demod2;
  CAX25Demodulator     m_demod3;
  uint16_t             m_lastFCS;
  uint32_t             m_count;
  uint32_t             m_slotTime;
  uint32_t             m_slotCount;
  uint8_t              m_pPersist;
  bool                 m_dcd;
  bool                 m_canTX;
  uint8_t              m_x;
  uint8_t              m_a;
  uint8_t              m_b;
  uint8_t              m_c;
  
  void initRand();
  uint8_t rand();
};

#endif

#endif

