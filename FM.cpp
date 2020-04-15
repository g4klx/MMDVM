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

void CFM::samples(bool cos, q15_t* samples, uint8_t length)
{
  // De-emphasis

  bool ctcss = m_goertzel.process(samples, length);

  bool validSignal = ctcss && cos;

  stateMachine(validSignal);

  if (m_modemState != STATE_FM)
    return;

  // Only let audio through when relaying audio
  if (m_state != FS_RELAYING) {
    for (uint8_t i = 0U; i < length; i++)
      samples[i] = 0;
  }

  m_rfAck.getAudio(samples, length);

  m_callsign.getAudio(samples, length);

  m_timeoutTone.getAudio(samples, length);

  // Band-pass filter

  m_ctcss.getAudio(samples, length);

  // Pre-emphasis

  io.write(STATE_FM, samples, length);
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

void CFM::stateMachine(bool validSignal)
{
  m_callsignTimer.clock();
  m_timeoutTimer.clock();
  m_holdoffTimer.clock();
  m_kerchunkTimer.clock();
  m_ackMinTimer.clock();
  m_ackDelayTimer.clock();
  m_hangTimer.clock();

  switch (m_state) {
    case FS_LISTENING:
      listeningState(validSignal);
      break;
    case FS_KERCHUNK:
      kerchunkState(validSignal);
      break;
    case FS_RELAYING:
      relayingState(validSignal);
      break;
    case FS_RELAYING_WAIT:
      relayingWaitState(validSignal);
      break;
    case FS_TIMEOUT:
      timeoutState(validSignal);
      break;
    case FS_TIMEOUT_WAIT:
      timeoutWaitState(validSignal);
      break;
    case FS_HANG:
      hangState(validSignal);
      break;
    default:
      break;
  }
}

void CFM::listeningState(bool validSignal)
{
  if (m_kerchunkTimer.getTimeout() > 0U) {
    m_state = FS_KERCHUNK;
    m_kerchunkTimer.start();
  } else {
    m_state = FS_RELAYING;
    if (m_callsignAtStart)
      sendCallsign();
  }

  beginRelaying();

  m_callsignTimer.start();

  m_modemState = STATE_FM;
}

void CFM::kerchunkState(bool validSignal)
{
  if (validSignal) {
    if (m_kerchunkTimer.hasExpired()) {
      m_state = FS_RELAYING;
      m_kerchunkTimer.stop();
    }
  } else {
    m_state = FS_LISTENING;
    m_kerchunkTimer.stop();
    m_timeoutTimer.stop();
    m_ackMinTimer.stop();
    m_callsignTimer.stop();
    m_holdoffTimer.stop();
    m_modemState = STATE_IDLE;
  }
}

void CFM::relayingState(bool validSignal)
{
  if (validSignal) {
    if (m_timeoutTimer.isRunning() && m_timeoutTimer.hasExpired()) {
      m_state = FS_TIMEOUT;
      m_ackMinTimer.stop();
      m_timeoutTimer.stop();
      m_timeoutTone.start();
    }
  } else {
    m_state = FS_RELAYING_WAIT;
    m_ackDelayTimer.start();
  }

  if (m_callsignTimer.isRunning() && m_callsignTimer.hasExpired()) {
    sendCallsign();
    m_callsignTimer.start();
  }
}

void CFM::relayingWaitState(bool validSignal)
{
  if (validSignal) {
    m_state = FS_RELAYING;
    m_ackDelayTimer.stop();
  } else {
    if (m_ackDelayTimer.isRunning() && m_ackDelayTimer.hasExpired()) {
      m_state = FS_HANG;

      if (m_ackMinTimer.isRunning()) {
        if (m_ackMinTimer.hasExpired()) {
          m_rfAck.start();
          m_ackMinTimer.stop();
        }
      } else {
          m_rfAck.start();
          m_ackMinTimer.stop();
      }

      m_ackDelayTimer.stop();
      m_timeoutTimer.stop();
      m_hangTimer.start();
    }
  }

  if (m_callsignTimer.isRunning() && m_callsignTimer.hasExpired()) {
    sendCallsign();
    m_callsignTimer.start();
  }
}

void CFM::hangState(bool validSignal)
{
  if (validSignal) {
    m_state = FS_RELAYING;
    m_rfAck.stop();
    beginRelaying();
  } else {
    if (m_hangTimer.isRunning() && m_hangTimer.hasExpired()) {
      m_state = FS_LISTENING;
      m_hangTimer.stop();

      if (m_callsignAtEnd)
        sendCallsign();

      m_callsignTimer.stop();
      m_holdoffTimer.stop();
      m_modemState = STATE_IDLE;
    }
  }

  if (m_callsignTimer.isRunning() && m_callsignTimer.hasExpired()) {
    sendCallsign();
    m_callsignTimer.start();
  }
}

void CFM::timeoutState(bool validSignal)
{
  if (!validSignal) {
    m_state = FS_TIMEOUT_WAIT;
    m_ackDelayTimer.start();
  }

  if (m_callsignTimer.isRunning() && m_callsignTimer.hasExpired()) {
    sendCallsign();
    m_callsignTimer.start();
  }
}

void CFM::timeoutWaitState(bool validSignal)
{
  if (validSignal) {
    m_state = FS_TIMEOUT;
    m_ackDelayTimer.stop();
  } else {
    if (m_ackDelayTimer.isRunning() && m_ackDelayTimer.hasExpired()) {
      m_state = FS_HANG;
      m_timeoutTone.stop();
      m_rfAck.start();
      m_ackDelayTimer.stop();
      m_ackMinTimer.stop();
      m_timeoutTimer.stop();
      m_hangTimer.start();
    }
  }

  if (m_callsignTimer.isRunning() && m_callsignTimer.hasExpired()) {
    sendCallsign();
    m_callsignTimer.start();
  }
}

void CFM::sendCallsign()
{
  if (m_holdoffTimer.isRunning()) {
    if (m_holdoffTimer.hasExpired()) {
      m_callsign.start();
      m_holdoffTimer.start();
    }
  } else {
    m_callsign.start();
  }
}

void CFM::beginRelaying()
{
  m_timeoutTimer.start();
  m_ackMinTimer.start();
}
