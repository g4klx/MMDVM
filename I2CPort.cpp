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

#if defined(MODE_OLED) || defined(I2C_REPEATER)

#include "I2CPort.h"

CI2CPort::CI2CPort(uint8_t n) :
m_n(n),
m_ok(false)
{
}

bool CI2CPort::init()
{
  switch (m_n) {
    case 1U:
      break;
    case 2U:
      break;
    case 3U:
      break;
    default:
      return m_ok;
  }

  return m_ok;
}

uint8_t CI2CPort::writeCommand(const uint8_t* data, uint8_t length)
{
  if (!m_ok)
    return 6U;

  return 6U;
}

uint8_t CI2CPort::writeData(const uint8_t* data, uint8_t length)
{
  if (!m_ok)
    return 6U;

  return 6U;
}

#endif

