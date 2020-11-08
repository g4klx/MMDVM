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

#if !defined(AX25Demodulator_H)
#define  AX25Demodulator_H

#include "AX25Frame.h"
#include "AX25Twist.h"

enum AX25_STATE {
  AX25_IDLE,
  AX25_SYNC,
  AX25_RECEIVE
};

class CAX25Demodulator {
public:
  CAX25Demodulator(int8_t n);

  bool process(q15_t* samples, uint8_t length, CAX25Frame& frame);

  void setTwist(int8_t n);

  bool isDCD();

private:
  CAX25Frame           m_frame;
  CAX25Twist           m_twist;
  arm_fir_instance_q15 m_lpfFilter;
  q15_t                m_lpfState[70U];     // NoTaps + BlockSize - 1, 48 + 20 - 1 plus some spare
  bool*                m_delayLine;
  uint16_t             m_delayPos;
  bool                 m_nrziState;
  arm_fir_instance_f32 m_pllFilter;
  float32_t            m_pllState[20U];     // NoTaps + BlockSize - 1, 7 + 1 - 1 plus some spare
  bool                 m_pllLast;
  uint8_t              m_pllBits;
  float32_t            m_pllCount;
  float32_t            m_pllJitter;
  bool                 m_pllDCD;
  float32_t            m_iirHistory[5U];
  uint16_t             m_hdlcOnes;
  bool                 m_hdlcFlag;
  uint16_t             m_hdlcBuffer;
  uint16_t             m_hdlcBits;
  AX25_STATE           m_hdlcState;

  bool delay(bool b);
  bool NRZI(bool b);
  bool PLL(bool b);
  bool HDLC(bool b);
  float32_t iir(float32_t input);
};

#endif

#endif

