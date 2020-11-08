/*
 *   Copyright (C) 2009,2010,2015,2020 by Jonathan Naylor G4KLX
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

#if defined(MODE_FM)

#include "Globals.h"
#include "FMTimer.h"

CFMTimer::CFMTimer() :
m_timeout(0U),
m_timer(0U)
{
}

void CFMTimer::setTimeout(uint16_t secs, uint32_t msecs)
{
  m_timeout = (secs * 24000U) + (msecs * 24U);
}

uint32_t CFMTimer::getTimeout() const
{
  return m_timeout / 24U;
}

void CFMTimer::start()
{
  if (m_timeout > 0U)
    m_timer = 1U;
}

void CFMTimer::stop()
{
  m_timer = 0U;
}

void CFMTimer::clock(uint8_t length)
{
  if (m_timer > 0U && m_timeout > 0U)
    m_timer += length;
}

bool CFMTimer::isRunning() const
{
  return m_timer > 0U;
}

bool CFMTimer::hasExpired() const
{
  if (m_timeout == 0U || m_timer == 0U)
    return false;

  if (m_timer > m_timeout)
    return true;

  return false;
}

#endif

