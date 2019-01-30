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

// POCSAG 600 Hz Test Pattern
q15_t sine600Hz[] = {
              0,   5125,  10125,  14875,  19259,  23169,  26509,  29195,  31163,  32363,
          32767,  32363,  31163,  29195,  26509,  23169,  19259,  14875,  10125,   5125,
              0,  -5126, -10126, -14876, -19260, -23170, -26510, -29196, -31164, -32364,
         -32767, -32364, -31164, -29196, -26510, -23170, -19260, -14876, -10126,  -5126,
         };

CCalPOCSAG::CCalPOCSAG() :
m_state(POCSAGCAL_IDLE)
{
}

void CCalPOCSAG::process()
{
  if (m_state == POCSAGCAL_IDLE)
    return;

  uint16_t space = io.getSpace();
  if (space <= 205U)
    return;

  io.write(STATE_POCSAG, sine600Hz, 40U);
  io.write(STATE_POCSAG, sine600Hz, 40U);
  io.write(STATE_POCSAG, sine600Hz, 40U);
  io.write(STATE_POCSAG, sine600Hz, 40U);
  io.write(STATE_POCSAG, sine600Hz, 40U);
}

uint8_t CCalPOCSAG::write(const uint8_t* data, uint8_t length)
{
  if (length != 1U)
    return 4U;

  m_state = data[0U] == 1U ? POCSAGCAL_TX : POCSAGCAL_IDLE;

  return 0U;
}
