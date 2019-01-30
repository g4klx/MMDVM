/*
 *   Copyright (C) 2019 by Florian Wolters DF2ET
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
#include "Globals.h"
#include "CalPOCSAG.h"


CCalPOCSAG::CCalPOCSAG() :
m_state(POCSAGCAL_IDLE)
{
}

void CCalPOCSAG::process()
{
  if (m_state == POCSAGCAL_IDLE)
    return;

  uint16_t space = io.getSpace();
  if (space <= 165U)
    return;

  pocsagTX.writeByte(0xAAU);
}

uint8_t CCalPOCSAG::write(const uint8_t* data, uint8_t length)
{
  if (length != 1U)
    return 4U;

  m_state = data[0U] == 1U ? POCSAGCAL_TX : POCSAGCAL_IDLE;

  return 0U;
}
