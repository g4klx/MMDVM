/*
 *   Copyright (C) 2015,2016 by Jonathan Naylor G4KLX
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

#if !defined(CALDSTARRX_H)
#define  CALDSTARRX_H

#include "Config.h"
#include "DStarDefines.h"

class CCalDStarRX {
public:
  CCalDStarRX();

  void samples(const q15_t* samples, uint8_t length);

private:
  uint32_t m_pll;
  bool     m_prev;
  uint32_t m_patternBuffer;
  q15_t    m_rxBuffer[3U * 8U];
  uint8_t  m_ptr;

  void    process(q15_t value);
};

#endif

