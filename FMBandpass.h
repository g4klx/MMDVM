/*
 *   Copyright (C) 2020 by Jonathan Naylor G4KLX
 *   Copyright (C) 2026 by Cort Buffington N0MJS
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

#if defined(MODE_FM)

#if !defined(FMBandpass_H)
#define  FMBandpass_H

#include "FMDirectForm1.h"

// 3rd order Cheby Filter 300 to 2700Hz, 0.2dB passband ripple, sampling rate 24kHz
class CFMBandpass {
public:
  CFMBandpass();

  void reset();

  q15_t filter(q15_t in);

private:
  CFMDirectFormI m_stage1;
  CFMDirectFormI m_stage2;
  CFMDirectFormI m_stage3;
};

#endif

#endif

