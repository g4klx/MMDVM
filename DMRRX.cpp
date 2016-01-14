/*
 *   Copyright (C) 2009-2015 by Jonathan Naylor G4KLX
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
#include "DMRRX.h"

CDMRRX::CDMRRX() :
m_slot1RX(false),
m_slot2RX(true),
m_slotNo(0U),
m_dcd1(false),
m_dcd2(false)
{
}

void CDMRRX::samples(const q15_t* samples, const uint8_t* control, uint8_t length)
{
  for (uint16_t i = 0U; i < length; i++) {
    if (control[i] == MARK_SLOT1 && m_slotNo != 1U) {
      m_slot2RX.stop();
      m_slot1RX.start();
      m_slotNo = 1U;
    } else if (control[i] == MARK_SLOT2 && m_slotNo != 2U) {
      m_slot1RX.stop();
      m_slot2RX.start();
      m_slotNo = 2U;
    } else if (control[i] == MARK_END && m_slotNo != 0U) {
      if (m_slotNo == 1U)
        m_slot1RX.stop();
      if (m_slotNo == 2U)
        m_slot2RX.stop();
      m_slotNo = 0U;
    }

    switch (m_slotNo) {
      case 1U:
        m_dcd1 = m_slot1RX.processSample(samples[i]);
        break;
      case 2U:
        m_dcd2 = m_slot2RX.processSample(samples[i]);
        break;
      default:
        break;
    }
  }

  io.setDecode(m_dcd1 || m_dcd2);
}

void CDMRRX::setColorCode(uint8_t colorCode)
{
  m_slot1RX.setColorCode(colorCode);
  m_slot2RX.setColorCode(colorCode);
}

void CDMRRX::reset()
{
  m_slot1RX.reset();
  m_slot2RX.reset();
  m_slotNo = 0U;
}

