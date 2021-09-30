/*
 *   Copyright (C) 2020,2021 by Jonathan Naylor G4KLX
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
#include "FM.h"

const uint16_t FM_TX_BLOCK_SIZE = 100U;
const uint16_t FM_SERIAL_BLOCK_SIZE = 80U;//this is the number of sample pairs to send over serial. One sample pair is 3bytes.
                                          //three times this value shall never exceed 252
const uint16_t FM_SERIAL_BLOCK_SIZE_BYTES = FM_SERIAL_BLOCK_SIZE * 3U;


CFM::CFM() :
m_callsign(),
m_rfAck(),
m_extAck(),
m_ctcssRX(),
m_ctcssTX(),
m_squelch(),
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
m_statusTimer(),
m_reverseTimer(),
m_needReverse(false),
m_filterStage1(  724,   1448,   724, 32768, -37895, 21352),//3rd order Cheby Filter 300 to 2700Hz, 0.2dB passband ripple, sampling rate 24kHz
m_filterStage2(32768,      0,-32768, 32768, -50339, 19052),
m_filterStage3(32768, -65536, 32768, 32768, -64075, 31460),
m_blanking(),
m_accessMode(1U),
m_linkMode(false),
m_cosInvert(false),
m_noiseSquelch(false),
m_rfAudioBoost(1U),
m_extAudioBoost(1U),
m_downSampler(400U),// 100 ms of audio
m_extEnabled(false),
m_rxLevel(1),
m_inputRFRB(2401U),   // 100ms of audio + 1 sample
m_outputRFRB(2400U),  // 100ms of audio
m_inputExtRB(),
m_rfSignal(false),
m_extSignal(false)
{
  m_statusTimer.setTimeout(1U, 0U);
  m_reverseTimer.setTimeout(0U, 150U);

  insertDelay(100U);
}

void CFM::samples(bool cos, q15_t* samples, uint8_t length)
{
  if (m_linkMode)
    linkSamples(cos, samples, length);
  else
    repeaterSamples(cos, samples, length);
}

void CFM::repeaterSamples(bool cos, q15_t* samples, uint8_t length)
{
  if (m_cosInvert)
    cos = !cos;

  clock(length);

  uint8_t i = 0U;
  for (; i < length; i++) {
    // ARMv7-M has hardware integer division 
    q15_t currentRFSample = q15_t((q31_t(samples[i]) << 8) / m_rxLevel);

    if (m_noiseSquelch)
      cos = m_squelch.process(currentRFSample);

    q15_t currentExtSample;
    bool inputExt = m_inputExtRB.getSample(currentExtSample);//always consume the external input data so it does not overflow
    inputExt = inputExt && m_extEnabled;

    switch (m_accessMode) {
      case 0U:
        if (!inputExt && !cos && m_modemState != STATE_FM)
          continue;
        else
          stateMachine(cos, inputExt);
        break;

      case 1U: {
          bool ctcss = m_ctcssRX.process(currentRFSample);

          // Delay the audio by 100ms to better match the CTCSS detector output
          m_inputRFRB.put(currentRFSample);
          m_inputRFRB.get(currentRFSample);

          if (!inputExt && !ctcss && m_modemState != STATE_FM) {
            // No CTCSS detected, just carry on
            continue;
          } else if ((inputExt || ctcss) && m_modemState != STATE_FM) {
            // We had CTCSS or external input
            stateMachine(ctcss, inputExt);
            if (m_state == FS_LISTENING)
              continue;
          } else {
            stateMachine(ctcss, inputExt);
          }
        }
        break;

      case 2U: {
          bool ctcss = m_ctcssRX.process(currentRFSample);
          if (!inputExt && !ctcss && m_modemState != STATE_FM) {
            // No CTCSS detected, just carry on
            continue;
          } else if ((inputExt || (ctcss && cos)) && m_modemState != STATE_FM) {
            // We had CTCSS or external input
            stateMachine(ctcss && cos, inputExt);
            if (m_state == FS_LISTENING)
              continue;
          } else {
            stateMachine(ctcss && cos, inputExt);
          }
        }
        break;

      default: {
          bool ctcss = m_ctcssRX.process(currentRFSample);
          if (!inputExt && !ctcss && m_modemState != STATE_FM) {
            // No CTCSS detected, just carry on
            continue;
          } else if ((inputExt || (ctcss && cos)) && m_modemState != STATE_FM) {
            // We had CTCSS or external input
            stateMachine(ctcss && cos, inputExt);
            if (m_state == FS_LISTENING)
              continue;
          } else {
            stateMachine(cos, inputExt);
          }
        }
        break;
    }

    if (m_modemState != STATE_FM)
      continue;

    if (m_state == FS_LISTENING && !m_rfAck.isWanted() && !m_extAck.isWanted() && !m_callsign.isWanted() && !m_reverseTimer.isRunning())
      continue;

    q15_t currentSample = currentRFSample;
    q15_t currentBoost  = m_rfAudioBoost;
    if (m_state == FS_RELAYING_EXT || m_state == FS_KERCHUNK_EXT) {
      currentSample = currentExtSample;
      currentBoost  = m_extAudioBoost;
    }

    // Only let RF audio through when relaying RF audio
    if (m_duplex) {
      if (m_state == FS_RELAYING_RF || m_state == FS_KERCHUNK_RF || m_state == FS_RELAYING_EXT || m_state == FS_KERCHUNK_EXT) {
        currentSample = m_blanking.process(currentSample);
        if (m_extEnabled && (m_state == FS_RELAYING_RF || m_state == FS_KERCHUNK_RF))
          m_downSampler.addSample(currentSample);

        currentSample *= currentBoost;
      } else {
        currentSample = 0;
      }
    } else {
        if (m_state == FS_RELAYING_EXT || m_state == FS_KERCHUNK_EXT) {
          currentSample *= currentBoost;
        } else {
          if (m_extEnabled && (m_state == FS_RELAYING_RF || m_state == FS_KERCHUNK_RF))
            m_downSampler.addSample(currentSample);
          continue; 
        }
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

    currentSample = m_filterStage3.filter(m_filterStage2.filter(m_filterStage1.filter(currentSample)));

    if (!m_callsign.isRunning() && !m_rfAck.isRunning() && !m_extAck.isRunning())
      currentSample += m_timeoutTone.getAudio();

    currentSample += m_ctcssTX.getAudio(m_reverseTimer.isRunning());

    m_outputRFRB.put(currentSample);
  }
}

void CFM::linkSamples(bool cos, q15_t* samples, uint8_t length)
{
  if (m_cosInvert)
    cos = !cos;

  clock(length);

  uint8_t i = 0U;
  for (; i < length; i++) {
    // ARMv7-M has hardware integer division 
    q15_t currentRFSample = q15_t((q31_t(samples[i]) << 8) / m_rxLevel);

    if (m_noiseSquelch)
      cos = m_squelch.process(currentRFSample);

    q15_t currentExtSample;
    bool inputExt = m_inputExtRB.getSample(currentExtSample);//always consume the external input data so it does not overflow
    inputExt = inputExt && m_extEnabled;

    switch (m_accessMode) {
      case 0U:
        if (!inputExt && !cos && m_modemState != STATE_FM)
          continue;
        else
          stateMachine(cos, inputExt);
        break;

      case 1U: {
          bool ctcss = m_ctcssRX.process(currentRFSample);

          // Delay the audio by 100ms to better match the CTCSS detector output
          m_inputRFRB.put(currentRFSample);
          m_inputRFRB.get(currentRFSample);

          if (!inputExt && !ctcss && m_modemState != STATE_FM) {
            // No CTCSS detected, just carry on
            continue;
          } else if ((inputExt || ctcss) && m_modemState != STATE_FM) {
            // We had CTCSS or external input
            stateMachine(ctcss, inputExt);
            if (m_state == FS_LISTENING)
              continue;
          } else {
            stateMachine(ctcss, inputExt);
          }
        }
        break;

      case 2U: {
          bool ctcss = m_ctcssRX.process(currentRFSample);
          if (!inputExt && !ctcss && m_modemState != STATE_FM) {
            // No CTCSS detected, just carry on
            continue;
          } else if ((inputExt || (ctcss && cos)) && m_modemState != STATE_FM) {
            // We had CTCSS or external input
            stateMachine(ctcss && cos, inputExt);
            if (m_state == FS_LISTENING)
              continue;
          } else {
            stateMachine(ctcss && cos, inputExt);
          }
        }
        break;

      default: {
          bool ctcss = m_ctcssRX.process(currentRFSample);
          if (!inputExt && !ctcss && m_modemState != STATE_FM) {
            // No CTCSS detected, just carry on
            continue;
          } else if ((inputExt || (ctcss && cos)) && m_modemState != STATE_FM) {
            // We had CTCSS or external input
            stateMachine(ctcss && cos, inputExt);
            if (m_state == FS_LISTENING)
              continue;
          } else {
            stateMachine(cos, inputExt);
          }
        }
        break;
    }

    if (m_modemState != STATE_FM)
      continue;

    if (m_rfSignal && m_extEnabled) {
      q15_t currentSample = m_blanking.process(currentRFSample);
      m_downSampler.addSample(currentSample);
    }

    if (!m_extSignal)
      continue;

    q15_t currentSample = currentExtSample * m_extAudioBoost;

    currentSample = m_filterStage3.filter(m_filterStage2.filter(m_filterStage1.filter(currentSample)));

    currentSample += m_ctcssTX.getAudio(m_reverseTimer.isRunning());

    m_outputRFRB.put(currentSample);
  }
}

void CFM::process()
{
  uint16_t space = io.getSpace();
  uint16_t length = m_outputRFRB.getData();

  if (space > 10U && length >= FM_TX_BLOCK_SIZE ) {
    space -= 2U;
    if (length > FM_TX_BLOCK_SIZE)
      length = FM_TX_BLOCK_SIZE;
    if (space > FM_TX_BLOCK_SIZE)
      space = FM_TX_BLOCK_SIZE;
    if (length > space)
      length = space;

    q15_t samples[FM_TX_BLOCK_SIZE];

    for (uint16_t i = 0U; i < length; i++) {
      q15_t sample = 0;
      m_outputRFRB.get(sample);
      samples[i] = sample;
    }

    io.write(STATE_FM, samples, length);
  }

  if (m_extEnabled) {
    uint16_t length = m_downSampler.getData();

    if (length >= FM_SERIAL_BLOCK_SIZE) {
      if (length > FM_SERIAL_BLOCK_SIZE)
        length = FM_SERIAL_BLOCK_SIZE;
        
      TSamplePairPack serialSamples[FM_SERIAL_BLOCK_SIZE];

      for (uint16_t j = 0U; j < length; j++)
        m_downSampler.getPackedData(serialSamples[j]);

      serial.writeFMData((uint8_t*)serialSamples, length * sizeof(TSamplePairPack));
    }
  }
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
  m_statusTimer.stop();
  m_reverseTimer.stop();

  m_ctcssRX.reset();
  m_rfAck.stop();
  m_extAck.stop();
  m_callsign.stop();
  m_timeoutTone.stop();

  m_outputRFRB.reset();
  m_inputExtRB.reset();

  m_downSampler.reset();
  m_squelch.reset();
  
  m_needReverse = false;
  m_rfSignal    = false;
  m_extSignal   = false;
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

uint8_t CFM::setMisc(uint16_t timeout, uint8_t timeoutLevel, uint8_t ctcssFrequency, uint8_t ctcssHighThreshold, uint8_t ctcssLowThreshold, uint8_t ctcssLevel, uint8_t kerchunkTime, uint8_t hangTime, uint8_t accessMode, bool linkMode, bool cosInvert, bool noiseSquelch, uint8_t squelchHighThreshold, uint8_t squelchLowThreshold, uint8_t rfAudioBoost, uint8_t maxDev, uint8_t rxLevel)
{
  m_accessMode   = accessMode;
  m_linkMode     = linkMode;
  m_cosInvert    = cosInvert;
  m_noiseSquelch = noiseSquelch;

  m_rfAudioBoost = q15_t(rfAudioBoost);

  m_timeoutTimer.setTimeout(timeout, 0U);

  m_kerchunkTimer.setTimeout(kerchunkTime, 0U);

  m_hangTimer.setTimeout(hangTime, 0U);

  m_timeoutTone.setParams(timeoutLevel);
  m_blanking.setParams(maxDev, timeoutLevel);

  m_rxLevel = rxLevel; //q15_t(255)/q15_t(rxLevel >> 1);

  m_squelch.setParams(squelchHighThreshold, squelchLowThreshold);

  uint8_t ret = m_ctcssRX.setParams(ctcssFrequency, ctcssHighThreshold, ctcssLowThreshold);
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
  if (m_linkMode) {
      linkStateMachine(validRFSignal, validExtSignal);
  } else {
    if (m_duplex)
      duplexStateMachine(validRFSignal, validExtSignal);
    else
      simplexStateMachine(validRFSignal, validExtSignal);
  }
}

void CFM::simplexStateMachine(bool validRFSignal, bool validExtSignal)
{
  switch (m_state) {
    case FS_LISTENING:
      listeningStateSimplex(validRFSignal, validExtSignal);
      break;
    case FS_RELAYING_RF:
      relayingRFStateSimplex(validRFSignal);
      break;
    case FS_RELAYING_WAIT_RF:
      relayingRFWaitStateSimplex(validRFSignal);
      break;
    case FS_TIMEOUT_RF:
      timeoutRFStateSimplex(validRFSignal);
      break;
    case FS_TIMEOUT_WAIT_RF:
      timeoutRFStateSimplex(validRFSignal);
      break;
    case FS_RELAYING_EXT:
      relayingExtStateSimplex(validExtSignal);
      break;
    case FS_RELAYING_WAIT_EXT:
      relayingExtWaitStateSimplex(validExtSignal);
      break;
    case FS_TIMEOUT_EXT:
      timeoutExtStateSimplex(validExtSignal);
      break;
    case FS_TIMEOUT_WAIT_EXT:
      timeoutExtWaitStateSimplex(validExtSignal);
      break;
    default:
      break;
  }

  if (m_state == FS_LISTENING) {
    if (!m_reverseTimer.isRunning() && m_needReverse)
      m_reverseTimer.start();

    if (m_reverseTimer.isRunning() && m_reverseTimer.hasExpired()) {
      m_reverseTimer.stop();
      m_needReverse = false;
    }
  }
}

void CFM::duplexStateMachine(bool validRFSignal, bool validExtSignal)
{
  switch (m_state) {
    case FS_LISTENING:
      listeningStateDuplex(validRFSignal, validExtSignal);
      break;
    case FS_KERCHUNK_RF:
      kerchunkRFStateDuplex(validRFSignal);
      break;
    case FS_RELAYING_RF:
      relayingRFStateDuplex(validRFSignal);
      break;
    case FS_RELAYING_WAIT_RF:
      relayingRFWaitStateDuplex(validRFSignal);
      break;
    case FS_TIMEOUT_RF:
      timeoutRFStateDuplex(validRFSignal);
      break;
    case FS_TIMEOUT_WAIT_RF:
      timeoutRFWaitStateDuplex(validRFSignal);
      break;
    case FS_KERCHUNK_EXT:
      kerchunkExtStateDuplex(validExtSignal);
      break;
    case FS_RELAYING_EXT:
      relayingExtStateDuplex(validExtSignal);
      break;
    case FS_RELAYING_WAIT_EXT:
      relayingExtWaitStateDuplex(validExtSignal);
      break;
    case FS_TIMEOUT_EXT:
      timeoutExtStateDuplex(validExtSignal);
      break;
    case FS_TIMEOUT_WAIT_EXT:
      timeoutExtWaitStateDuplex(validExtSignal);
      break;
    case FS_HANG:
      hangStateDuplex(validRFSignal, validExtSignal);
      break;
    default:
      break;
  }

  if (m_state == FS_LISTENING && !m_rfAck.isWanted() && !m_extAck.isWanted() && !m_callsign.isWanted()) {
    if (!m_reverseTimer.isRunning() && m_needReverse)
      m_reverseTimer.start();

    if (m_reverseTimer.isRunning() && m_reverseTimer.hasExpired()) {
      m_reverseTimer.stop();
      m_needReverse = false;
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
  m_statusTimer.clock(length);
  m_reverseTimer.clock(length);

  if (m_statusTimer.isRunning() && m_statusTimer.hasExpired()) {
    serial.writeFMStatus(m_state);
    m_statusTimer.start();
  }
}

void CFM::listeningStateDuplex(bool validRFSignal, bool validExtSignal)
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

    if (m_state == FS_RELAYING_RF || m_state == FS_KERCHUNK_RF) {
      insertSilence(50U);

      beginRelaying();

      m_callsignTimer.start();
      m_reverseTimer.stop();

      io.setDecode(true);
      io.setADCDetection(true);

      m_statusTimer.start();
      serial.writeFMStatus(m_state);
    }
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

    if (m_state == FS_RELAYING_EXT || m_state == FS_KERCHUNK_EXT) {
      insertSilence(50U);

      beginRelaying();

      m_callsignTimer.start();
      m_reverseTimer.stop();

      m_statusTimer.start();
      serial.writeFMStatus(m_state);
    }
  }
}

void CFM::listeningStateSimplex(bool validRFSignal, bool validExtSignal)
{
  if (validRFSignal) {
    DEBUG1("State to RELAYING_RF");
    m_state = FS_RELAYING_RF;

    io.setDecode(true);
    io.setADCDetection(true);

    m_timeoutTimer.start();
    m_reverseTimer.stop();

    m_statusTimer.start();
    serial.writeFMStatus(m_state);
  } else if (validExtSignal) {
    DEBUG1("State to RELAYING_EXT");
    m_state = FS_RELAYING_EXT;

    insertSilence(50U);

    m_timeoutTimer.start();
    m_reverseTimer.stop();

    m_statusTimer.start();
    serial.writeFMStatus(m_state);
  }
}

void CFM::kerchunkRFStateDuplex(bool validSignal)
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
    io.setDecode(false);
    io.setADCDetection(false);

    DEBUG1("State to LISTENING");
    m_state = FS_LISTENING;
    m_kerchunkTimer.stop();
    m_timeoutTimer.stop();
    m_ackMinTimer.stop();
    m_callsignTimer.stop();
    m_statusTimer.stop();
    m_needReverse = true;
    if (m_extEnabled)
      serial.writeFMEOT();
  }
}

void CFM::relayingRFStateDuplex(bool validSignal)
{
  if (validSignal) {
    if (m_timeoutTimer.isRunning() && m_timeoutTimer.hasExpired()) {
      DEBUG1("State to TIMEOUT_RF");
      m_state = FS_TIMEOUT_RF;
      m_ackMinTimer.stop();
      m_timeoutTimer.stop();
      m_timeoutTone.start();

      if (m_extEnabled)
        serial.writeFMEOT();
    }
  } else {
    io.setDecode(false);
    io.setADCDetection(false);

    DEBUG1("State to RELAYING_WAIT_RF");
    m_state = FS_RELAYING_WAIT_RF;
    m_ackDelayTimer.start();

    if (m_extEnabled)
      serial.writeFMEOT();
  }

  if (m_callsignTimer.isRunning() && m_callsignTimer.hasExpired()) {
    sendCallsign();
    m_callsignTimer.start();
  }
}

void CFM::relayingRFStateSimplex(bool validSignal)
{
  if (validSignal) {
    if (m_timeoutTimer.isRunning() && m_timeoutTimer.hasExpired()) {
      DEBUG1("State to TIMEOUT_RF");
      m_state = FS_TIMEOUT_RF;

      m_timeoutTimer.stop();

      if (m_extEnabled)
        serial.writeFMEOT();
    }
  } else {
    io.setDecode(false);
    io.setADCDetection(false);

    DEBUG1("State to RELAYING_WAIT_RF");
    m_state = FS_RELAYING_WAIT_RF;
    m_ackDelayTimer.start();

    if (m_extEnabled)
      serial.writeFMEOT();
  }
}

void CFM::relayingRFWaitStateDuplex(bool validSignal)
{
  if (validSignal) {
    io.setDecode(true);
    io.setADCDetection(true);

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

void CFM::relayingRFWaitStateSimplex(bool validSignal)
{
  if (validSignal) {
    io.setDecode(true);
    io.setADCDetection(true);

    DEBUG1("State to RELAYING_RF");
    m_state = FS_RELAYING_RF;
    m_ackDelayTimer.stop();
  } else {
    if (m_ackDelayTimer.isRunning() && m_ackDelayTimer.hasExpired()) {
      DEBUG1("State to LISTENING");
      m_state = FS_LISTENING;
      m_ackDelayTimer.stop();
      m_timeoutTimer.stop();
    }
  }
}

void CFM::kerchunkExtStateDuplex(bool validSignal)
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
    m_statusTimer.stop();
    m_needReverse = true;
  }
}

void CFM::relayingExtStateDuplex(bool validSignal)
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

void CFM::relayingExtStateSimplex(bool validSignal)
{
  if (validSignal) {
    if (m_timeoutTimer.isRunning() && m_timeoutTimer.hasExpired()) {
      DEBUG1("State to TIMEOUT_EXT");
      m_state = FS_TIMEOUT_EXT;

      m_timeoutTimer.stop();
    }
  } else {
    DEBUG1("State to RELAYING_WAIT_EXT");
    m_state = FS_RELAYING_WAIT_EXT;
    m_ackDelayTimer.start();
  }
}

void CFM::relayingExtWaitStateDuplex(bool validSignal)
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

void CFM::relayingExtWaitStateSimplex(bool validSignal)
{
  if (validSignal) {
    DEBUG1("State to RELAYING_EXT");
    m_state = FS_RELAYING_EXT;
    m_ackDelayTimer.stop();
  } else {
    if (m_ackDelayTimer.isRunning() && m_ackDelayTimer.hasExpired()) {
      DEBUG1("State to LISTENING");
      m_state = FS_LISTENING;
      m_ackDelayTimer.stop();
      m_timeoutTimer.stop();
      m_needReverse = true;
    }
  }
}

void CFM::hangStateDuplex(bool validRFSignal, bool validExtSignal)
{
  if (validRFSignal) {
    io.setDecode(true);
    io.setADCDetection(true);

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
      m_statusTimer.stop();

      if (m_callsignAtEnd)
        sendCallsign();

      m_callsignTimer.stop();
      m_needReverse = true;
    }
  }

  if (m_callsignTimer.isRunning() && m_callsignTimer.hasExpired()) {
    sendCallsign();
    m_callsignTimer.start();
  }
}

void CFM::timeoutRFStateDuplex(bool validSignal)
{
  if (!validSignal) {
    io.setDecode(false);
    io.setADCDetection(false);

    DEBUG1("State to TIMEOUT_WAIT_RF");
    m_state = FS_TIMEOUT_WAIT_RF;

    if (m_callsignAtEnd)
        sendCallsign();

    m_ackDelayTimer.start();
  }

  if (m_callsignTimer.isRunning() && m_callsignTimer.hasExpired()) {
    sendCallsign();
    m_callsignTimer.start();
  }
}

void CFM::timeoutRFStateSimplex(bool validSignal)
{
  if (!validSignal) {
    io.setDecode(false);
    io.setADCDetection(false);

    DEBUG1("State to TIMEOUT_WAIT_RF");
    m_state = FS_TIMEOUT_WAIT_RF;

    m_ackDelayTimer.start();
  }
}

void CFM::timeoutRFWaitStateDuplex(bool validSignal)
{
  if (validSignal) {
    io.setDecode(true);
    io.setADCDetection(true);

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

void CFM::timeoutRFWaitStateSimplex(bool validSignal)
{
  if (validSignal) {
    io.setDecode(true);
    io.setADCDetection(true);

    DEBUG1("State to TIMEOUT_RF");
    m_state = FS_TIMEOUT_RF;
    m_ackDelayTimer.stop();
  } else {
    if (m_ackDelayTimer.isRunning() && m_ackDelayTimer.hasExpired()) {
      DEBUG1("State to LISTENING");
      m_state = FS_LISTENING;
      m_ackDelayTimer.stop();
      m_timeoutTimer.stop();
    }
  }
}

void CFM::timeoutExtStateDuplex(bool validSignal)
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

void CFM::timeoutExtStateSimplex(bool validSignal)
{
  if (!validSignal) {
    DEBUG1("State to TIMEOUT_WAIT_EXT");
    m_state = FS_TIMEOUT_WAIT_EXT;
    m_ackDelayTimer.start();
  }
}

void CFM::timeoutExtWaitStateDuplex(bool validSignal)
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

void CFM::timeoutExtWaitStateSimplex(bool validSignal)
{
  if (validSignal) {
    DEBUG1("State to TIMEOUT_EXT");
    m_state = FS_TIMEOUT_EXT;
    m_ackDelayTimer.stop();
  } else {
    if (m_ackDelayTimer.isRunning() && m_ackDelayTimer.hasExpired()) {
      DEBUG1("State to LISTENING");
      m_state = FS_LISTENING;
      m_ackDelayTimer.stop();
      m_timeoutTimer.stop();
      m_needReverse = true;
    }
  }
}

void CFM::linkStateMachine(bool validRFSignal, bool validExtSignal)
{
  if (validRFSignal && !m_rfSignal) {
    io.setDecode(true);
    io.setADCDetection(true);

    if (!m_extSignal) {
      DEBUG1("State to RELAYING_RF");
      m_state = FS_RELAYING_RF;
      m_statusTimer.start();
      serial.writeFMStatus(m_state);
    }

    m_rfSignal = true;
  }

  if (validExtSignal && !m_extSignal) {
    if (!m_rfSignal) {
      DEBUG1("State to RELAYING_EXT");
      m_state = FS_RELAYING_EXT;
      m_statusTimer.start();
      serial.writeFMStatus(m_state);
    }

    insertSilence(50U);

    m_extSignal = true;
  }

  if (!validRFSignal && m_rfSignal) {
    io.setDecode(false);
    io.setADCDetection(false);

    if (!m_extSignal) {
      DEBUG1("State to LISTENING");
      m_state = FS_LISTENING;
      m_statusTimer.stop();
    }

    m_rfSignal = false;

    if (m_extEnabled)
      serial.writeFMEOT();
  }

  if (!validExtSignal && m_extSignal) {
    if (!m_rfSignal) {
      DEBUG1("State to LISTENING");
      m_state = FS_LISTENING;
      m_statusTimer.stop();
    }

    m_needReverse = true;
    m_extSignal   = false;
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
  // The amount of free space for receiving external audio, in frames.
  return m_inputExtRB.getSpace() / FM_SERIAL_BLOCK_SIZE_BYTES;
}

uint8_t CFM::writeData(const uint8_t* data, uint8_t length)
{
  //todo check if length is a multiple of 3
  m_inputExtRB.addData(data, length);
  return 0U;
}

void CFM::insertDelay(uint16_t ms)
{
  uint32_t nSamples = ms * 24U;

  for (uint32_t i = 0U; i < nSamples; i++)
    m_inputRFRB.put(0);
}

void CFM::insertSilence(uint16_t ms)
{
  uint32_t nSamples = ms * 24U;

  for (uint32_t i = 0U; i < nSamples; i++)
    m_outputRFRB.put(0);
}

#endif

