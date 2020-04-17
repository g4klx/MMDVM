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

q15_t FILTER_COEFFS[] = {
     -4,     0,     1,    -2,    -9,   -15,   -16,    -11,    -2,     4,     6,     1,    -7,   -15,
    -17,    -9,     3,    16,    21,    14,     0,    -12,   -14,    -1,    21,    43,    51,    41,
     19,    -1,    -5,    14,    50,    85,    98,     81,    44,     9,     0,    28,    80,   130,
    147,   120,    60,     2,   -17,    16,    88,    157,   178,   132,    39,   -52,   -90,   -49,
     46,   140,   166,    96,   -42,  -182,  -245,   -195,   -63,    67,   106,     7,  -194,  -399,
   -496,  -429,  -238,   -41,    26,  -106,  -396,   -697,  -843,  -743,  -444,  -121,    12,  -165,
   -603, -1084, -1329, -1163,  -629,    -6,   320,     67,  -759, -1803, -2474, -2204,  -739,  1695,
   4421,  6556,  7363,  6556,  4421,  1695,  -739,  -2204, -2474, -1803,  -759,    67,   320,    -6,
   -629, -1163, -1329, -1084,  -603,  -165,    12,   -121,  -444,  -743,  -843,  -697,  -396,  -106,
     26,   -41,  -238,  -429,  -496,  -399,  -194,      7,   106,    67,   -63,  -195,  -245,  -182,
    -42,    96,   166,   140,    46,   -49,   -90,    -52,    39,   132,   178,   157,    88,    16,
    -17,     2,    60,   120,   147,   130,    80,     28,     0,     9,    44,    81,    98,    85,
     50,    14,    -5,    -1,    19,    41,    51,     43,    21,    -1,   -14,   -12,     0,    14,
     21,    16,     3,    -9,   -17,   -15,    -7,      1,     6,     4,    -2,   -11,   -16,   -15,
     -9,    -2,     1,     0,    -4};

const uint16_t FILTER_COEFFS_LEN = 201U;

CFM::CFM() :
m_filter(),
m_filterState(),
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
  ::memset(m_filterState, 0x00U, 230U * sizeof(q15_t));

  m_filter.numTaps = FILTER_COEFFS_LEN;
  m_filter.pState  = m_filterState;
  m_filter.pCoeffs = FILTER_COEFFS;
}

void CFM::samples(q15_t* samples, uint8_t length)
{
  bool validSignal = m_goertzel.process(samples, length);

  stateMachine(validSignal, length);

  if (m_modemState != STATE_FM)
    return;

  // Only let audio through when relaying audio
  if (m_state != FS_RELAYING) {
    for (uint8_t i = 0U; i < length; i++)
      samples[i] = 0;
  }

  if (!m_callsign.isRunning())
    m_rfAck.getAudio(samples, length);

  if (!m_rfAck.isRunning())
    m_callsign.getAudio(samples, length);

  if (!m_callsign.isRunning() && !m_rfAck.isRunning())
    m_timeoutTone.getAudio(samples, length);

  q15_t output[RX_BLOCK_SIZE];
  ::arm_fir_fast_q15(&m_filter, samples, output, length);

  m_ctcss.getAudio(output, length);

  io.write(STATE_FM, output, length);
}

void CFM::process()
{
}

void CFM::reset()
{
}

uint8_t CFM::setCallsign(const char* callsign, uint8_t speed, uint16_t frequency, uint8_t time, uint8_t holdoff, uint8_t highLevel, uint8_t lowLevel, bool callsignAtStart, bool callsignAtEnd)
{
  m_callsignAtStart = callsignAtStart;
  m_callsignAtEnd   = callsignAtEnd;

  uint16_t holdoffTime  = 0U;
  uint16_t callsignTime = time * 60U;
  if (holdoff > 0U)
    holdoffTime  = callsignTime / holdoff;

  m_holdoffTimer.setTimeout(holdoffTime, 0U);
  m_callsignTimer.setTimeout(callsignTime, 0U);

  return m_callsign.setParams(callsign, speed, frequency, lowLevel);
}

uint8_t CFM::setAck(const char* rfAck, uint8_t speed, uint16_t frequency, uint8_t minTime, uint16_t delay, uint8_t level)
{
  m_ackDelayTimer.setTimeout(0U, delay);
  m_ackMinTimer.setTimeout(minTime, 0U);

  return m_rfAck.setParams(rfAck, speed, frequency, level);
}

uint8_t CFM::setMisc(uint16_t timeout, uint8_t timeoutLevel, uint8_t ctcssFrequency, uint8_t ctcssThreshold, uint8_t ctcssLevel, uint8_t kerchunkTime, uint8_t hangTime)
{
  m_timeoutTimer.setTimeout(timeout, 0U);
  m_kerchunkTimer.setTimeout(kerchunkTime, 0U);
  m_hangTimer.setTimeout(hangTime, 0U);

  m_timeoutTone.setParams(timeoutLevel);

  m_goertzel.setParams(ctcssFrequency, ctcssThreshold);

  return m_ctcss.setParams(ctcssFrequency, ctcssLevel);
}

void CFM::stateMachine(bool validSignal, uint8_t length)
{
  m_callsignTimer.clock(length);
  m_timeoutTimer.clock(length);
  m_holdoffTimer.clock(length);
  m_kerchunkTimer.clock(length);
  m_ackMinTimer.clock(length);
  m_ackDelayTimer.clock(length);
  m_hangTimer.clock(length);

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

  if (m_state == FS_LISTENING && m_modemState == STATE_FM) {
    if (!m_callsign.isRunning() && !m_rfAck.isRunning()) {
      DEBUG1("Change to STATE_IDLE");
      m_modemState = STATE_IDLE;
    }
  }
}

void CFM::listeningState(bool validSignal)
{
  if (m_kerchunkTimer.getTimeout() > 0U) {
    DEBUG1("State to KERCHUNK");
    m_state = FS_KERCHUNK;
    m_kerchunkTimer.start();
  } else {
    DEBUG1("State to RELAYING");
    m_state = FS_RELAYING;
    if (m_callsignAtStart)
      sendCallsign();
  }

  beginRelaying();

  m_callsignTimer.start();

  DEBUG1("Change to STATE_FM");
  m_modemState = STATE_FM;
}

void CFM::kerchunkState(bool validSignal)
{
  if (validSignal) {
    if (m_kerchunkTimer.hasExpired()) {
      DEBUG1("State to RELAYING");
      m_state = FS_RELAYING;
      m_kerchunkTimer.stop();
    }
  } else {
    DEBUG1("State to LISTENING");
    m_state = FS_LISTENING;
    m_kerchunkTimer.stop();
    m_timeoutTimer.stop();
    m_ackMinTimer.stop();
    m_callsignTimer.stop();
    m_holdoffTimer.stop();
  }
}

void CFM::relayingState(bool validSignal)
{
  if (validSignal) {
    if (m_timeoutTimer.isRunning() && m_timeoutTimer.hasExpired()) {
      DEBUG1("State to TIMEOUT");
      m_state = FS_TIMEOUT;
      m_ackMinTimer.stop();
      m_timeoutTimer.stop();
      m_timeoutTone.start();
    }
  } else {
    DEBUG1("State to RELAYING_WAIT");
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
    DEBUG1("State to RELAYING");
    m_state = FS_RELAYING;
    m_ackDelayTimer.stop();
  } else {
    if (m_ackDelayTimer.isRunning() && m_ackDelayTimer.hasExpired()) {
      DEBUG1("State to HANG");
      m_state = FS_HANG;

      if (m_ackMinTimer.isRunning()) {
        if (m_ackMinTimer.hasExpired()) {
          DEBUG1("Send ack");
          m_rfAck.start();
          m_ackMinTimer.stop();
        }
      } else {
          DEBUG1("Send ack");
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
    DEBUG1("State to RELAYING");
    m_state = FS_RELAYING;
    DEBUG1("Stop ack");
    m_rfAck.stop();
    beginRelaying();
  } else {
    if (m_hangTimer.isRunning() && m_hangTimer.hasExpired()) {
      DEBUG1("State to LISTENING");
      m_state = FS_LISTENING;
      m_hangTimer.stop();

      if (m_callsignAtEnd)
        sendCallsign();

      m_callsignTimer.stop();
      m_holdoffTimer.stop();
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
    DEBUG1("State to TIMEOUT_WAIT");
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
    DEBUG1("State to TIMEOUT");
    m_state = FS_TIMEOUT;
    m_ackDelayTimer.stop();
  } else {
    if (m_ackDelayTimer.isRunning() && m_ackDelayTimer.hasExpired()) {
      DEBUG1("State to HANG");
      m_state = FS_HANG;
      m_timeoutTone.stop();
      DEBUG1("Send ack");
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
      DEBUG1("Send callsign");
      m_callsign.start();
      m_holdoffTimer.start();
    }
  } else {
    DEBUG1("Send callsign");
    m_callsign.start();
  }
}

void CFM::beginRelaying()
{
  m_timeoutTimer.start();
  m_ackMinTimer.start();
}
