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
#include "AX25TX.h"


CAX25TX::CAX25TX()
{
}

void CAX25TX::process()
{
}

uint8_t CAX25TX::writeData(const uint8_t* data, uint16_t length)
{
  return 0U;
}

void CAX25TX::setParams(int8_t twist)
{
}

void CAX25TX::setTXDelay(uint8_t delay)
{
}

uint8_t CAX25TX::getSpace() const
{
  return 0U;
}

