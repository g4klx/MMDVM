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

#if !defined(FMCTCSSRX_H)
#define  FMCTCSSRX_H

#include "Config.h"

enum CTCSSState
{
  CTS_NONE  = 0,
  CTS_READY = 1,
  CTS_VALID = 2,
  CTS_READY_VALID = CTS_READY | CTS_VALID
};

inline CTCSSState operator|(CTCSSState a, CTCSSState b)
{
  return static_cast<CTCSSState>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline CTCSSState operator&(CTCSSState a, CTCSSState b)
{
  return static_cast<CTCSSState>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

inline CTCSSState operator~(CTCSSState a)
{
  return static_cast<CTCSSState>(~(static_cast<uint8_t>(a)));
}

#define CTCSS_READY(a) ((a & CTS_READY) != 0)
#define CTCSS_NOT_READY(a) ((a & CTS_READY) == 0)
#define CTCSS_VALID(a) ((a & CTS_VALID) != 0)
#define CTCSS_NOT_VALID(a) ((a & CTS_VALID) == 0)

class CFMCTCSSRX {
public:
  CFMCTCSSRX();

  uint8_t setParams(uint8_t frequency, uint8_t threshold);
  
  CTCSSState process(q15_t samples);

  CTCSSState getState();

  void reset();

private:
 q31_t      m_coeffDivTwo;
 q31_t      m_threshold;
 uint16_t   m_count;
 q31_t      m_q0;
 q31_t      m_q1;
 CTCSSState m_result;
};

#endif
