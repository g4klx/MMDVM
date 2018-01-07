/*
 *   Copyright (C) 2018 by Andy Uribe CA6JAU
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

#if !defined(CALP25_H)
#define  CALP25_H

#include "Config.h"
#include "P25Defines.h"

enum P25CAL1K {
  P25CAL1K_IDLE,
  P25CAL1K_LDU1,
  P25CAL1K_LDU2
};

class CCalP25 {
public:
  CCalP25();

  void process();

  uint8_t write(const uint8_t* data, uint8_t length);

private:
  bool      m_transmit;
  P25CAL1K  m_state;
};

#endif

