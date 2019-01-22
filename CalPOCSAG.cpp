/*
 *   Copyright (C) 2018 by Florian Wolters DF2ET
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

CCalPOCSAG::CCalPOCSAG() :
m_transmit(false),
m_state(POCSAGCAL_IDLE),
m_audioSeq(0U)
{
}

void CCalPOCSAG::process()
{
  pocsagTX.process();

  uint16_t space = pocsagTX.getSpace();
  if (space < 1U)
    return;

  switch (m_state) {
    case POCSAGCAL_TX:
      //pocsagTX.writeData(POCSAG_CAL[m_audioSeq], POCSAG_FRAME_LENGTH_BYTES + 1U);
      m_audioSeq = (m_audioSeq + 1U) % 4U;
      if(!m_transmit)
        m_state = POCSAGCAL_IDLE;
      break;
    default:
        m_state = POCSAGCAL_IDLE;
        m_audioSeq = 0U;
      break;
  }
}

uint8_t CCalPOCSAG::write(const uint8_t* data, uint8_t length)
{
  if (length != 1U)
    return 4U;

  m_transmit = data[0U] == 1U;

  if(m_transmit && m_state == POCSAGCAL_IDLE)
    m_state = POCSAGCAL_TX;

  return 0U;
}

