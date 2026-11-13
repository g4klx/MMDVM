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

#include "Globals.h"
#include "FMBandpass.h"

CFMBandpass::CFMBandpass() :
m_stage1(  724,   1448,   724, 32768, -37895, 21352),
m_stage2(32768,      0,-32768, 32768, -50339, 19052),
m_stage3(32768, -65536, 32768, 32768, -64075, 31460)
{
}

void CFMBandpass::reset()
{
  m_stage1.reset();
  m_stage2.reset();
  m_stage3.reset();
}

// keep the fractional bits between the stages, round once at the end
q15_t CFMBandpass::filter(q15_t in)
{
  q31_t out = m_stage3.filter(m_stage2.filter(m_stage1.filter(q31_t(in) << FM_FILTER_FRAC_BITS)));

  return q15_t(__SSAT((out + (1 << (FM_FILTER_FRAC_BITS - 1))) >> FM_FILTER_FRAC_BITS, 16));
}

#endif

