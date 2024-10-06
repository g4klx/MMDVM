/*
 *   Copyright (C) 2018 by Andy Uribe CA6JAU
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

#if defined(MODE_NXDN)

#if !defined(CALNXDN_H)
#define  CALNXDN_H

enum NXDNCAL1K {
  NXDNCAL1K_IDLE,
  NXDNCAL1K_TX
};

class CCalNXDN {
public:
  CCalNXDN();

  void process();

  uint8_t write(const uint8_t* data, uint16_t length);

private:
  bool      m_transmit;
  NXDNCAL1K m_state;
  uint8_t   m_audioSeq;
};

#endif

#endif

