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
     26,     19,      6,     -7,    -13,     -7,      8,     27,     35,     22,    -10,    -48,    -70,    -60,    -21,     24,
     41,      3,    -84,   -185,   -243,   -222,   -129,    -23,     15,    -66,   -257,   -471,   -591,   -539,   -332,    -94,
      9,   -134,   -503,   -926,  -1160,  -1035,   -570,     -5,    300,     64,   -728,  -1747,  -2418,  -2171,   -732,   1688,
   4417,   6564,   7376,   6564,   4417,   1688,   -732,  -2171,  -2418,  -1747,   -728,     64,    300,     -5,   -570,  -1035,
  -1160,   -926,   -503,   -134,      9,    -94,   -332,   -539,   -591,   -471,   -257,    -66,     15,    -23,   -129,   -222,
   -243,   -185,    -84,      3,     41,     24,    -21,    -60,    -70,    -48,    -10,     22,     35,     27,      8,     -7,
    -13,     -7,      6,     19,     26};

const uint16_t FILTER_COEFFS_LEN = 101U;


CFM::CFM() :
m_filter(),
m_filterState(),
m_callsign(),
m_rfAck(),
m_ctcssRX(),
m_ctcssTX(),
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
  uint8_t i = 0;
  for (; i < length; i++) {
    q15_t currentSample = samples[i];//save to a local variable to avoid indirection on every access

    CTCSSState ctcssState = m_ctcssRX.process(currentSample);

    if (CTCSS_NOT_READY(ctcssState) && m_modemState != STATE_FM) {
      //Not enough samples to determine if you have CTCSS, just carry on
      continue;
    } else if (CTCSS_READY(ctcssState) && m_modemState != STATE_FM) {
      //we had enough samples for CTCSS and we are in some other mode than FM
      bool validCTCSS = CTCSS_VALID(ctcssState);
      stateMachine(validCTCSS, i + 1U);
      if (m_modemState != STATE_FM)
        continue;
    } else if (CTCSS_READY(ctcssState) && m_modemState == STATE_FM) {
      //We had enough samples for CTCSS and we are in FM mode, trigger the state machine
      bool validCTCSS = CTCSS_VALID(ctcssState);
      stateMachine(validCTCSS, i + 1U);
      if (m_modemState != STATE_FM)
        break;
    } else if (CTCSS_NOT_READY(ctcssState) && m_modemState == STATE_FM && i == length - 1) {
      //Not enough samples for CTCSS but we already are in FM, trigger the state machine
      //but do not trigger the state machine on every single sample, save CPU!
        bool validCTCSS = CTCSS_VALID(ctcssState);
        stateMachine(validCTCSS, i + 1U);
    }
    
    // Only let audio through when relaying audio
    if (m_state != FS_RELAYING && m_state != FS_KERCHUNK)
      currentSample = 0U;

    if (!m_callsign.isRunning())
      currentSample += m_rfAck.getAudio();
    
    if (!m_rfAck.isRunning())
      currentSample += m_callsign.getAudio();

    if (!m_callsign.isRunning() && !m_rfAck.isRunning())
      currentSample += m_timeoutTone.getAudio();

    //ToDo Filtering
    //::arm_fir_fast_q15(&m_filter, samples + i, &currentSample, 1);

    currentSample += m_ctcssTX.getAudio();

    samples[i] = currentSample;
  }

  if (m_modemState == STATE_FM)
    io.write(STATE_FM, samples, i);//only write the actual number of processed samples to IO
}

void CFM::process()
{
}

void CFM::reset()
{
  m_ctcssRX.reset();
}

uint8_t CFM::setCallsign(const char* callsign, uint8_t speed, uint16_t frequency, uint8_t time, uint8_t holdoff, uint8_t level, bool callsignAtStart, bool callsignAtEnd)
{
  m_callsignAtStart = callsignAtStart;
  m_callsignAtEnd   = callsignAtEnd;

  uint16_t holdoffTime  = 0U;
  uint16_t callsignTime = time * 60U;
  if (holdoff > 0U)
    holdoffTime  = callsignTime / holdoff;

  m_holdoffTimer.setTimeout(holdoffTime, 0U);
  m_callsignTimer.setTimeout(callsignTime, 0U);

  return m_callsign.setParams(callsign, speed, frequency, level);
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

  uint8_t ret = m_ctcssRX.setParams(ctcssFrequency, ctcssThreshold);
  if (ret != 0U)
    return ret;

  return m_ctcssTX.setParams(ctcssFrequency, ctcssLevel);
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
      m_callsignTimer.stop();
      m_timeoutTimer.stop();
      m_holdoffTimer.stop();
      m_kerchunkTimer.stop();
      m_ackMinTimer.stop();
      m_ackDelayTimer.stop();
      m_hangTimer.stop();
    }
  }
}

void CFM::listeningState(bool validSignal)
{
  if (validSignal) {
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
