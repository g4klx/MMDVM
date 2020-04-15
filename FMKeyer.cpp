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
#include "FMKeyer.h"

CFMKeyer::CFMKeyer() :
m_level(128 * 128),
m_wanted(false),
m_running(false)
{
}

void CFMKeyer::setParams(const char* text, uint8_t speed, uint16_t frequency, uint8_t level)
{
  m_level = q15_t(level * 128);
}

void CFMKeyer::getAudio(q15_t* samples, uint8_t length)
{
}

void CFMKeyer::start()
{
  m_wanted = true;
}

void CFMKeyer::stop()
{
}

bool CFMKeyer::isRunning() const
{
  return m_running;
}
