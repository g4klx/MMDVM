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

#if !defined(AX25Frame_H)
#define  AX25Frame_H

const uint16_t AX25_MAX_PACKET_LEN = 300U;

class CAX25Frame {
public:
  CAX25Frame(const uint8_t* data, uint16_t length);
  CAX25Frame();

  bool append(uint16_t c);

  bool checkCRC();

  void addCRC();

  uint8_t  m_data[AX25_MAX_PACKET_LEN];
  uint16_t m_length;
  uint16_t m_fcs;
};

#endif

#endif

