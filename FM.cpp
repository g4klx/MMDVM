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

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT_AUDIO(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])
#define READ_BIT_AUDIO(p,i)    (p[(i)>>3] & BIT_MASK_TABLE[(i)&7])

CFM::CFM() :
m_callsign(),
m_rfAck(),
m_extAck(),
m_ctcssRX(),
m_ctcssTX(),
m_timeoutTone(),
m_state(FS_LISTENING),
m_callsignAtStart(false),
m_callsignAtEnd(false),
m_callsignAtLatch(false),
m_callsignTimer(),
m_timeoutTimer(),
m_holdoffTimer(),
m_kerchunkTimer(),
m_ackMinTimer(),
m_ackDelayTimer(),
m_hangTimer(),
m_filterStage1(  724,   1448,   724, 32768, -37895, 21352),//3rd order Cheby Filter 300 to 2700Hz, 0.2dB passband ripple, sampling rate 24kHz
m_filterStage2(32768,      0,-32768, 32768, -50339, 19052),
m_filterStage3(32768, -65536, 32768, 32768, -64075, 31460),
m_blanking(),
m_useCOS(true),
m_cosInvert(false),
m_rfAudioBoost(1U),
m_extAudioBoost(1U),
m_downsampler(128U),   //Size might need adjustement
m_extEnabled(false),
m_rxLevel(1)
{
}

void CFM::samples(bool cos, q15_t* samples, uint8_t length)
{
  if (m_useCOS) {
    if (m_cosInvert)
      cos = !cos;
  } else {
    cos = true;
  }

  clock(length);

  uint8_t i = 0U;
  for (; i < length; i++) {
    // ARMv7-M has hardware integer division 
    q15_t currentSample = q15_t((q31_t(samples[i]) << 8) / m_rxLevel);

    uint8_t ctcssState = m_ctcssRX.process(currentSample);

    if (CTCSS_NOT_READY(ctcssState) && m_modemState != STATE_FM) {
      //Not enough samples to determine if you have CTCSS, just carry on
      continue;
    } else if (CTCSS_READY(ctcssState) && m_modemState != STATE_FM) {
      //we had enough samples for CTCSS and we are in some other mode than FM
      bool validCTCSS = CTCSS_VALID(ctcssState);
      // XXX Need to have somewhere to get the ext audio state
      stateMachine(validCTCSS && cos, false);
      if (m_modemState != STATE_FM)
        continue;
    } else if (CTCSS_READY(ctcssState) && m_modemState == STATE_FM) {
      //We had enough samples for CTCSS and we are in FM mode, trigger the state machine
      bool validCTCSS = CTCSS_VALID(ctcssState);
      // XXX Need to have somewhere to get the ext audio state
      stateMachine(validCTCSS && cos, false);
      if (m_modemState != STATE_FM)
        break;
    } else if (CTCSS_NOT_READY(ctcssState) && m_modemState == STATE_FM && i == length - 1) {
      //Not enough samples for CTCSS but we already are in FM, trigger the state machine
      //but do not trigger the state machine on every single sample, save CPU!
      bool validCTCSS = CTCSS_VALID(ctcssState);
      // XXX Need to have somewhere to get the ext audio state
      stateMachine(validCTCSS && cos, false);
    }

    // Only let RF audio through when relaying RF audio
    if (m_state == FS_RELAYING_RF || m_state == FS_KERCHUNK_RF) {
      if (m_extEnabled)
        m_downsampler.addSample(currentSample);

      currentSample = m_blanking.process(currentSample);
      currentSample *= m_rfAudioBoost;
    } else if (m_state == FS_RELAYING_EXT || m_state == FS_KERCHUNK_EXT) {
      // XXX Where do we receive the ext audio?
      currentSample = m_blanking.process(currentSample);
      currentSample *= m_extAudioBoost;
    } else {
      currentSample = 0;
    }

    if (!m_callsign.isRunning() && !m_extAck.isRunning())
      currentSample += m_rfAck.getHighAudio();

    if (!m_callsign.isRunning() && !m_rfAck.isRunning())
      currentSample += m_extAck.getHighAudio();

    if (!m_rfAck.isRunning() && !m_extAck.isRunning()) {
      if (m_state == FS_LISTENING)
        currentSample += m_callsign.getHighAudio();
      else
        currentSample += m_callsign.getLowAudio();
    }

    if (!m_callsign.isRunning() && !m_rfAck.isRunning() && !m_extAck.isRunning())
      currentSample += m_timeoutTone.getAudio();

    currentSample = m_filterStage3.filter(m_filterStage2.filter(m_filterStage1.filter(currentSample)));

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
  m_state = FS_LISTENING;

  m_callsignTimer.stop();
  m_timeoutTimer.stop();
  m_kerchunkTimer.stop();
  m_ackMinTimer.stop();
  m_ackDelayTimer.stop();
  m_hangTimer.stop();

  m_ctcssRX.reset();
  m_rfAck.stop();
  m_extAck.stop();
  m_callsign.stop();
  m_timeoutTone.stop();
}

uint8_t CFM::setCallsign(const char* callsign, uint8_t speed, uint16_t frequency, uint8_t time, uint8_t holdoff, uint8_t highLevel, uint8_t lowLevel, bool callsignAtStart, bool callsignAtEnd, bool callsignAtLatch)
{
  m_callsignAtStart = callsignAtStart;
  m_callsignAtEnd   = callsignAtEnd;
  m_callsignAtLatch = callsignAtLatch;

  uint16_t holdoffTime  = holdoff * 60U;
  uint16_t callsignTime = time * 60U;

  m_holdoffTimer.setTimeout(holdoffTime, 0U);
  m_callsignTimer.setTimeout(callsignTime, 0U);

  if (holdoffTime > 0U)
    m_holdoffTimer.start();

  return m_callsign.setParams(callsign, speed, frequency, highLevel, lowLevel);
}

uint8_t CFM::setAck(const char* rfAck, uint8_t speed, uint16_t frequency, uint8_t minTime, uint16_t delay, uint8_t level)
{
  m_ackDelayTimer.setTimeout(0U, delay);

  if (minTime > 0U)
    m_ackMinTimer.setTimeout(minTime, delay);

  return m_rfAck.setParams(rfAck, speed, frequency, level, level);
}

uint8_t CFM::setMisc(uint16_t timeout, uint8_t timeoutLevel, uint8_t ctcssFrequency, uint8_t ctcssThreshold, uint8_t ctcssLevel, uint8_t kerchunkTime, uint8_t hangTime, bool useCOS, bool cosInvert, uint8_t rfAudioBoost, uint8_t maxDev, uint8_t rxLevel)
{
  m_useCOS    = useCOS;
  m_cosInvert = cosInvert;

  m_rfAudioBoost = q15_t(rfAudioBoost);

  m_timeoutTimer.setTimeout(timeout, 0U);
  m_kerchunkTimer.setTimeout(kerchunkTime, 0U);
  m_hangTimer.setTimeout(hangTime, 0U);

  m_timeoutTone.setParams(timeoutLevel);
  m_blanking.setParams(maxDev, timeoutLevel);

  m_rxLevel = rxLevel; //q15_t(255)/q15_t(rxLevel >> 1);

  uint8_t ret = m_ctcssRX.setParams(ctcssFrequency, ctcssThreshold);
  if (ret != 0U)
    return ret;

  return m_ctcssTX.setParams(ctcssFrequency, ctcssLevel);
}

uint8_t CFM::setExt(const char* ack, uint8_t audioBoost, uint8_t speed, uint16_t frequency, uint8_t level)
{
  m_extEnabled = true;

  m_extAudioBoost = q15_t(audioBoost);

  return m_extAck.setParams(ack, speed, frequency, level, level);
}

void CFM::stateMachine(bool validRFSignal, bool validExtSignal)
{
  switch (m_state) {
    case FS_LISTENING:
      listeningState(validRFSignal, validExtSignal);
      break;
    case FS_KERCHUNK_RF:
      kerchunkRFState(validRFSignal);
      break;
    case FS_RELAYING_RF:
      relayingRFState(validRFSignal);
      break;
    case FS_RELAYING_WAIT_RF:
      relayingRFWaitState(validRFSignal);
      break;
    case FS_TIMEOUT_RF:
      timeoutRFState(validRFSignal);
      break;
    case FS_TIMEOUT_WAIT_RF:
      timeoutRFWaitState(validRFSignal);
      break;
    case FS_KERCHUNK_EXT:
      kerchunkExtState(validExtSignal);
      break;
    case FS_RELAYING_EXT:
      relayingExtState(validExtSignal);
      break;
    case FS_RELAYING_WAIT_EXT:
      relayingExtWaitState(validExtSignal);
      break;
    case FS_TIMEOUT_EXT:
      timeoutExtState(validExtSignal);
      break;
    case FS_TIMEOUT_WAIT_EXT:
      timeoutExtWaitState(validExtSignal);
      break;
    case FS_HANG:
      hangState(validRFSignal, validExtSignal);
      break;
    default:
      break;
  }

  if (m_state == FS_LISTENING && m_modemState == STATE_FM) {
    if (!m_callsign.isRunning() && !m_rfAck.isRunning() && !m_extAck.isRunning()) {
      DEBUG1("Change to STATE_IDLE");
      m_modemState = STATE_IDLE;
      m_callsignTimer.stop();
      m_timeoutTimer.stop();
      m_kerchunkTimer.stop();
      m_ackMinTimer.stop();
      m_ackDelayTimer.stop();
      m_hangTimer.stop();
    }
  }
}

void CFM::clock(uint8_t length)
{
  m_callsignTimer.clock(length);
  m_timeoutTimer.clock(length);
  m_holdoffTimer.clock(length);
  m_kerchunkTimer.clock(length);
  m_ackMinTimer.clock(length);
  m_ackDelayTimer.clock(length);
  m_hangTimer.clock(length);
}

void CFM::listeningState(bool validRFSignal, bool validExtSignal)
{
  if (validRFSignal) {
    if (m_kerchunkTimer.getTimeout() > 0U) {
      DEBUG1("State to KERCHUNK_RF");
      m_state = FS_KERCHUNK_RF;
      m_kerchunkTimer.start();
      if (m_callsignAtStart && !m_callsignAtLatch)
        sendCallsign();
    } else {
      DEBUG1("State to RELAYING_RF");
      m_state = FS_RELAYING_RF;
      if (m_callsignAtStart)
        sendCallsign();
    }

    beginRelaying();

    m_callsignTimer.start();

    DEBUG1("Change to STATE_FM");
    m_modemState = STATE_FM;
  } else if (validExtSignal) {
    if (m_kerchunkTimer.getTimeout() > 0U) {
      DEBUG1("State to KERCHUNK_EXT");
      m_state = FS_KERCHUNK_EXT;
      m_kerchunkTimer.start();
      if (m_callsignAtStart && !m_callsignAtLatch)
        sendCallsign();
    } else {
      DEBUG1("State to RELAYING_EXT");
      m_state = FS_RELAYING_EXT;
      if (m_callsignAtStart)
        sendCallsign();
    }

    beginRelaying();

    m_callsignTimer.start();

    DEBUG1("Change to STATE_FM");
    m_modemState = STATE_FM;
  }
}

void CFM::kerchunkRFState(bool validSignal)
{
  if (validSignal) {
    if (m_kerchunkTimer.hasExpired()) {
      DEBUG1("State to RELAYING_RF");
      m_state = FS_RELAYING_RF;
      m_kerchunkTimer.stop();
      if (m_callsignAtStart && m_callsignAtLatch) {
        sendCallsign();
        m_callsignTimer.start();
      }
    }
  } else {
    DEBUG1("State to LISTENING");
    m_state = FS_LISTENING;
    m_kerchunkTimer.stop();
    m_timeoutTimer.stop();
    m_ackMinTimer.stop();
    m_callsignTimer.stop();
  }
}

void CFM::relayingRFState(bool validSignal)
{
  if (validSignal) {
    if (m_timeoutTimer.isRunning() && m_timeoutTimer.hasExpired()) {
      DEBUG1("State to TIMEOUT_RF");
      m_state = FS_TIMEOUT_RF;
      m_ackMinTimer.stop();
      m_timeoutTimer.stop();
      m_timeoutTone.start();
    }
  } else {
    DEBUG1("State to RELAYING_WAIT_RF");
    m_state = FS_RELAYING_WAIT_RF;
    m_ackDelayTimer.start();
  }

  if (m_callsignTimer.isRunning() && m_callsignTimer.hasExpired()) {
    sendCallsign();
    m_callsignTimer.start();
  }
}

void CFM::relayingRFWaitState(bool validSignal)
{
  if (validSignal) {
    DEBUG1("State to RELAYING_RF");
    m_state = FS_RELAYING_RF;
    m_ackDelayTimer.stop();
  } else {
    if (m_ackDelayTimer.isRunning() && m_ackDelayTimer.hasExpired()) {
      DEBUG1("State to HANG");
      m_state = FS_HANG;

      if (m_ackMinTimer.isRunning()) {
        if (m_ackMinTimer.hasExpired()) {
          DEBUG1("Send RF ack");
          m_rfAck.start();
          m_ackMinTimer.stop();
        }
      } else {
          DEBUG1("Send RF ack");
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

void CFM::kerchunkExtState(bool validSignal)
{
  if (validSignal) {
    if (m_kerchunkTimer.hasExpired()) {
      DEBUG1("State to RELAYING_EXT");
      m_state = FS_RELAYING_EXT;
      m_kerchunkTimer.stop();
      if (m_callsignAtStart && m_callsignAtLatch) {
        sendCallsign();
        m_callsignTimer.start();
      }
    }
  } else {
    DEBUG1("State to LISTENING");
    m_state = FS_LISTENING;
    m_kerchunkTimer.stop();
    m_timeoutTimer.stop();
    m_ackMinTimer.stop();
    m_callsignTimer.stop();
  }
}

void CFM::relayingExtState(bool validSignal)
{
  if (validSignal) {
    if (m_timeoutTimer.isRunning() && m_timeoutTimer.hasExpired()) {
      DEBUG1("State to TIMEOUT_EXT");
      m_state = FS_TIMEOUT_EXT;
      m_ackMinTimer.stop();
      m_timeoutTimer.stop();
      m_timeoutTone.start();
    }
  } else {
    DEBUG1("State to RELAYING_WAIT_EXT");
    m_state = FS_RELAYING_WAIT_EXT;
    m_ackDelayTimer.start();
  }

  if (m_callsignTimer.isRunning() && m_callsignTimer.hasExpired()) {
    sendCallsign();
    m_callsignTimer.start();
  }
}

void CFM::relayingExtWaitState(bool validSignal)
{
  if (validSignal) {
    DEBUG1("State to RELAYING_EXT");
    m_state = FS_RELAYING_EXT;
    m_ackDelayTimer.stop();
  } else {
    if (m_ackDelayTimer.isRunning() && m_ackDelayTimer.hasExpired()) {
      DEBUG1("State to HANG");
      m_state = FS_HANG;

      if (m_ackMinTimer.isRunning()) {
        if (m_ackMinTimer.hasExpired()) {
          DEBUG1("Send Ext ack");
          m_extAck.start();
          m_ackMinTimer.stop();
        }
      } else {
          DEBUG1("Send Ext ack");
          m_extAck.start();
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

void CFM::hangState(bool validRFSignal, bool validExtSignal)
{
  if (validRFSignal) {
    DEBUG1("State to RELAYING_RF");
    m_state = FS_RELAYING_RF;
    DEBUG1("Stop ack");
    m_rfAck.stop();
    m_extAck.stop();
    beginRelaying();
  } else if (validExtSignal) {
    DEBUG1("State to RELAYING_EXT");
    m_state = FS_RELAYING_EXT;
    DEBUG1("Stop ack");
    m_rfAck.stop();
    m_extAck.stop();
    beginRelaying();
  } else {
    if (m_hangTimer.isRunning() && m_hangTimer.hasExpired()) {
      DEBUG1("State to LISTENING");
      m_state = FS_LISTENING;
      m_hangTimer.stop();

      if (m_callsignAtEnd)
        sendCallsign();

      m_callsignTimer.stop();
    }
  }

  if (m_callsignTimer.isRunning() && m_callsignTimer.hasExpired()) {
    sendCallsign();
    m_callsignTimer.start();
  }
}

void CFM::timeoutRFState(bool validSignal)
{
  if (!validSignal) {
    DEBUG1("State to TIMEOUT_WAIT_RF");
    m_state = FS_TIMEOUT_WAIT_RF;
    m_ackDelayTimer.start();
  }

  if (m_callsignTimer.isRunning() && m_callsignTimer.hasExpired()) {
    sendCallsign();
    m_callsignTimer.start();
  }
}

void CFM::timeoutRFWaitState(bool validSignal)
{
  if (validSignal) {
    DEBUG1("State to TIMEOUT_RF");
    m_state = FS_TIMEOUT_RF;
    m_ackDelayTimer.stop();
  } else {
    if (m_ackDelayTimer.isRunning() && m_ackDelayTimer.hasExpired()) {
      DEBUG1("State to HANG");
      m_state = FS_HANG;
      m_timeoutTone.stop();
      DEBUG1("Send RF ack");
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

void CFM::timeoutExtState(bool validSignal)
{
  if (!validSignal) {
    DEBUG1("State to TIMEOUT_WAIT_EXT");
    m_state = FS_TIMEOUT_WAIT_EXT;
    m_ackDelayTimer.start();
  }

  if (m_callsignTimer.isRunning() && m_callsignTimer.hasExpired()) {
    sendCallsign();
    m_callsignTimer.start();
  }
}

void CFM::timeoutExtWaitState(bool validSignal)
{
  if (validSignal) {
    DEBUG1("State to TIMEOUT_EXT");
    m_state = FS_TIMEOUT_EXT;
    m_ackDelayTimer.stop();
  } else {
    if (m_ackDelayTimer.isRunning() && m_ackDelayTimer.hasExpired()) {
      DEBUG1("State to HANG");
      m_state = FS_HANG;
      m_timeoutTone.stop();
      DEBUG1("Send Ext ack");
      m_extAck.start();
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

uint8_t CFM::getSpace() const
{
  // The amount of free space for receiving external audio, in bytes.
  return 0U;
}

uint8_t CFM::writeData(const uint8_t* data, uint8_t length)
{
  q15_t samples[170U];
  uint8_t nSamples = 0U;

  for (uint8_t i = 0U; i < length; i += 3U) {
    uint16_t sample1 = 0U;
    uint16_t sample2 = 0U;
    uint16_t MASK = 0x0001U;

    uint8_t* base = data + i;
    for (uint8_t j = 0U; j < 12U; j++, MASK <<= 1) {
      uint8_t pos1 = j;
      uint8_t pos2 = j + 12U;

      bool b1 = READ_BIT_AUDIO(base, pos1) != 0U;
      bool b2 = READ_BIT_AUDIO(base, pos2) != 0U;

      if (b1)
        sample1 |= MASK;
      if (b2)
        sample2 |= MASK;
    }

    // Convert from uint16_t (0 - +4095) to Q15 (-2048 - +2047)
    samples[nSamples++] = q15_t(sample1) - 2048;
    samples[nSamples++] = q15_t(sample2) - 2048;
  }

  // Received audio is now in Q15 format in samples, with length nSamples.

  return 0U;
}
