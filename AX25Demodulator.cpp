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
#include "AX25Demodulator.h"

CAX25Demodulator::CAX25Demodulator() :
m_nrziState(false)
{
}

bool CAX25Demodulator::process(const q15_t* samples, uint8_t length, AX25Frame& frame)
{
  return false;
}

bool CAX25Demodulator::NRZI(bool b)
{
  bool result = (b == m_nrziState);

  m_nrziState = b;

  return result;
}

