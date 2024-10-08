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

#if !defined(FM_H)
#define  FM_H

#include "FMBlanking.h"
#include "FMCTCSSRX.h"
#include "FMCTCSSTX.h"
#include "FMTimeout.h"
#include "FMKeyer.h"
#include "FMTimer.h"
#include "RingBuffer.h"
#include "FMDirectForm1.h"
#include "FMDownSampler.h"
#include "FMUpSampler.h"
#include "FMNoiseSquelch.h"

enum FM_STATE {
  FS_LISTENING,
  FS_KERCHUNK_RF,
  FS_RELAYING_RF,
  FS_RELAYING_WAIT_RF,
  FS_TIMEOUT_RF,
  FS_TIMEOUT_WAIT_RF,
  FS_KERCHUNK_EXT,
  FS_RELAYING_EXT,
  FS_RELAYING_WAIT_EXT,
  FS_TIMEOUT_EXT,
  FS_TIMEOUT_WAIT_EXT,
  FS_HANG
};


class CFM {
public:
  CFM();

  void samples(bool cos, q15_t* samples, uint8_t length);

  void process();

  void reset();

  uint8_t setCallsign(const char* callsign, uint8_t speed, uint16_t frequency, uint8_t time, uint8_t holdoff, uint8_t highLevel, uint8_t lowLevel, bool callsignAtStart, bool callsignAtEnd, bool callsignAtLatch);
  uint8_t setAck(const char* rfAck, uint8_t speed, uint16_t frequency, uint8_t minTime, uint16_t delay, uint8_t level);
  uint8_t setMisc(uint16_t timeout, uint8_t timeoutLevel, uint8_t ctcssFrequency, uint8_t ctcssHighThreshold, uint8_t ctcssLowThreshold, uint8_t ctcssLevel, uint8_t kerchunkTime, uint8_t hangTime, uint8_t accessMode, bool linkMode, bool cosInvert, bool noiseSquelch, uint8_t squelchHighThreshold, uint8_t squelchLowThreshold, uint8_t rfAudioBoost, uint8_t maxDev, uint8_t rxLevel);
  uint8_t setExt(const char* ack, uint8_t audioBoost, uint8_t speed, uint16_t frequency, uint8_t level);

  uint8_t getSpace() const;

  uint8_t writeData(const uint8_t* data, uint8_t length);

private:
  CFMKeyer             m_callsign;
  CFMKeyer             m_rfAck;
  CFMKeyer             m_extAck;
  CFMCTCSSRX           m_ctcssRX;
  CFMCTCSSTX           m_ctcssTX;
  CFMNoiseSquelch      m_squelch;
  CFMTimeout           m_timeoutTone;
  FM_STATE             m_state;
  bool                 m_callsignAtStart;
  bool                 m_callsignAtEnd;
  bool                 m_callsignAtLatch;
  CFMTimer             m_callsignTimer;
  CFMTimer             m_timeoutTimer;
  CFMTimer             m_holdoffTimer;
  CFMTimer             m_kerchunkTimer;
  CFMTimer             m_ackMinTimer;
  CFMTimer             m_ackDelayTimer;
  CFMTimer             m_hangTimer;
  CFMTimer             m_statusTimer;
  CFMTimer             m_reverseTimer;
  bool                 m_needReverse;
  CFMDirectFormI       m_filterStage1;
  CFMDirectFormI       m_filterStage2;
  CFMDirectFormI       m_filterStage3;
  CFMBlanking          m_blanking;
  uint8_t              m_accessMode;
  bool                 m_linkMode;
  bool                 m_cosInvert;
  bool                 m_noiseSquelch;
  q15_t                m_rfAudioBoost;
  q15_t                m_extAudioBoost;
  CFMDownSampler       m_downSampler;
  bool                 m_extEnabled;
  q15_t                m_rxLevel;
  CRingBuffer<q15_t>   m_inputRFRB;
  CRingBuffer<q15_t>   m_outputRFRB;
  CFMUpSampler         m_inputExtRB;
  bool                 m_rfSignal;
  bool                 m_extSignal;

  void stateMachine(bool validRFSignal, bool validExtSignal);

  void repeaterSamples(bool cos, q15_t* samples, uint8_t length);
  void linkSamples(bool cos, q15_t* samples, uint8_t length);

  void duplexStateMachine(bool validRFSignal, bool validExtSignal);
  void listeningStateDuplex(bool validRFSignal, bool validExtSignal);
  void kerchunkRFStateDuplex(bool validSignal);
  void relayingRFStateDuplex(bool validSignal);
  void relayingRFWaitStateDuplex(bool validSignal);
  void timeoutRFStateDuplex(bool validSignal);
  void timeoutRFWaitStateDuplex(bool validSignal);
  void kerchunkExtStateDuplex(bool validSignal);
  void relayingExtStateDuplex(bool validSignal);
  void relayingExtWaitStateDuplex(bool validSignal);
  void timeoutExtStateDuplex(bool validSignal);
  void timeoutExtWaitStateDuplex(bool validSignal);
  void hangStateDuplex(bool validRFSignal, bool validExtSignal);

  void simplexStateMachine(bool validRFSignal, bool validExtSignal);
  void listeningStateSimplex(bool validRFSignal, bool validExtSignal);
  void relayingRFStateSimplex(bool validSignal);
  void relayingRFWaitStateSimplex(bool validSignal);
  void timeoutRFStateSimplex(bool validSignal);
  void timeoutRFWaitStateSimplex(bool validSignal);
  void relayingExtStateSimplex(bool validSignal);
  void relayingExtWaitStateSimplex(bool validSignal);
  void timeoutExtStateSimplex(bool validSignal);
  void timeoutExtWaitStateSimplex(bool validSignal);

  void linkStateMachine(bool validRFSignal, bool validExtSignal);

  void clock(uint8_t length);

  void sendCallsign();
  void beginRelaying();

  void insertDelay(uint16_t ms);
  void insertSilence(uint16_t ms);
};

#endif

#endif

