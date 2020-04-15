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
#include "FM.h"

CFM::CFM() :
m_callsign(),
m_rfAck(),
m_goertzel(),
m_ctcss(),
m_timeoutTone(),
m_state(FS_LISTENING),
m_callsignAtStart(false),
m_callsignAtEnd(false),
m_callsignTimer(),
m_timeoutTimer(),
m_holdoffTimer(),
m_kerchunkTimer(),
m_ackMinTimer(),
m_ackDelayTimer(),
m_hangTimer()
{
}

void CFM::samples(bool cos, const q15_t* samples, uint8_t length)
{
}

void CFM::process()
{
}

void CFM::reset()
{
}

void CFM::setCallsign(const char* callsign, uint8_t speed, uint16_t frequency, uint8_t time, uint8_t holdoff, uint8_t highLevel, uint8_t lowLevel, bool callsignAtStart, bool callsignAtEnd)
{
  m_callsign.setParams(callsign, speed, frequency, lowLevel);

  m_callsignAtStart = callsignAtStart;
  m_callsignAtEnd   = callsignAtEnd;

  m_holdoffTimer.setTimeout(holdoff);
}

void CFM::setAck(const char* rfAck, uint8_t speed, uint16_t frequency, uint8_t minTime, uint16_t delay, uint8_t level)
{
  m_rfAck.setParams(rfAck, speed, frequency, level);

  m_ackDelayTimer.setTimeout(delay);
  m_ackMinTimer.setTimeout(minTime);
}

void CFM::setMisc(uint16_t timeout, uint8_t timeoutLevel, uint8_t ctcssFrequency, uint8_t ctcssThreshold, uint8_t ctcssLevel, uint8_t kerchunkTime, uint8_t hangTime)
{
  m_timeoutTone.setParams(timeoutLevel);
  m_goertzel.setParams(ctcssFrequency, ctcssThreshold);
  m_ctcss.setParams(ctcssFrequency, ctcssLevel);

  m_timeoutTimer.setTimeout(timeout);
  m_kerchunkTimer.setTimeout(kerchunkTime);
  m_hangTimer.setTimeout(hangTime);
}
