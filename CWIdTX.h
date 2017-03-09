/*
 *   Copyright (C) 2009-2015 by Jonathan Naylor G4KLX
 *   Copyright (C) 2016 by Colin Durbridge G4EML
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

#if !defined(CWIDTX_H)
#define  CWIDTX_H

#include "Config.h"

class CCWIdTX {
public:
  CCWIdTX();

  void process();

  uint8_t write(const uint8_t* data, uint8_t length);

  void reset();

private:
  uint8_t  m_poBuffer[1000U];
  uint16_t m_poLen;
  uint16_t m_poPtr;
  uint8_t  m_n;
};

#endif

