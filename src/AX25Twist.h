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

#if !defined(AX25Twist_H)
#define  AX25Twist_H

class CAX25Twist {
public:
  CAX25Twist(int8_t n);

  void process(q15_t* in, q15_t* out, uint8_t length);

  void setTwist(int8_t n);

private:
  arm_fir_instance_q15 m_filter;
  q15_t                m_state[40U];   // NoTaps + BlockSize - 1, 9 + 20 - 1 plus some spare
};

#endif

#endif

