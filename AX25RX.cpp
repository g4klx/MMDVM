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
#include "Globals.h"
#include "AX25RX.h"

CAX25RX::CAX25RX() :
m_demod1(),
m_demod2(),
m_demod3(),
m_lastFCS(0U)
{
}

void CAX25RX::samples(const q15_t* samples, uint8_t length)
{
  AX25Frame frame;

  bool ret = m_demod1.process(samples, length, frame);
  if (ret) {
    if (m_lastFCS != frame.m_fcs) {
      DEBUG1("Decoder 1 reported");
      m_lastFCS = frame.m_fcs;
      serial.writeAX25Data(frame.m_data, frame.m_length);
    }
  }

  ret = m_demod2.process(samples, length, frame);
  if (ret) {
    if (m_lastFCS != frame.m_fcs) {
      DEBUG1("Decoder 2 reported");
      m_lastFCS = frame.m_fcs;
      serial.writeAX25Data(frame.m_data, frame.m_length);
    }
  }

  ret = m_demod3.process(samples, length, frame);
  if (ret) {
    if (m_lastFCS != frame.m_fcs) {
      DEBUG1("Decoder 3 reported");
      m_lastFCS = frame.m_fcs;
      serial.writeAX25Data(frame.m_data, frame.m_length);
    }
  }
}

