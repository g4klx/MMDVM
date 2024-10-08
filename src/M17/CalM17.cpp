/*
 *   Copyright (C) 2009-2015,2020,2021 by Jonathan Naylor G4KLX
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

#include "Config.h"

#if defined(MODE_M17)

#include "Globals.h"
#include "CalM17.h"
#include "M17Defines.h"

const uint8_t PREAMBLE[] = {0x00U,
	0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U,
	0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U,
	0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U,
	0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U,
	0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U,
	0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U, 0x77U};

CCalM17::CCalM17() :
m_transmit(false)
{
}

void CCalM17::process()
{
  m17TX.process();

  if (!m_transmit)
    return;

  uint16_t space = m17TX.getSpace();
  if (space < 2U)
    return;

  m17TX.writeData(PREAMBLE, M17_FRAME_LENGTH_BYTES + 1U);
}

uint8_t CCalM17::write(const uint8_t* data, uint16_t length)
{
  if (length != 1U)
    return 4U;

  m_transmit = data[0U] == 1U;

  return 0U;
}

#endif

