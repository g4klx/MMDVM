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

#if defined(MODE_FM)

#if !defined(FMBlanking_H)
#define  FMBlanking_H

class CFMBlanking {
public:
  CFMBlanking();

  void setParams(uint8_t value, uint8_t level);

  q15_t process(q15_t sample);

private:
  q15_t    m_posValue;
  q15_t    m_negValue;
  q15_t    m_level;
  bool     m_running;
  uint32_t m_pos;
  uint8_t  m_n;
};

#endif

#endif

