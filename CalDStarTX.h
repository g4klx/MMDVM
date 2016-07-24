/*
 *   Copyright (C) 2015,2016 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#if !defined(CALDSTARTX_H)
#define  CALDSTARTX_H

#include "Config.h"
#include "DStarDefines.h"

class CCalDStarTX {
public:
  CCalDStarTX();

  uint8_t write(const uint8_t* data, uint8_t length);

  void process();

private:
  bool      m_transmit;
  uint16_t  m_count;
};

#endif

