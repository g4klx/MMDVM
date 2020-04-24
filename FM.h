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

#if !defined(FM_H)
#define  FM_H

#include "Config.h"

#include "FMCTCSSRX.h"
#include "FMCTCSSTX.h"
#include "FMTimeout.h"
#include "FMKeyer.h"
#include "FMTimer.h"
#include "FMDirectForm1.h"

enum FM_STATE {
  FS_LISTENING,
  FS_KERCHUNK,
  FS_RELAYING,
  FS_RELAYING_WAIT,
  FS_TIMEOUT,
  FS_TIMEOUT_WAIT,
  FS_HANG
};




class CFM {
public:
  CFM();

  void samples(q15_t* samples, uint8_t length);

  void process();

  void reset();

  uint8_t setCallsign(const char* callsign, uint8_t speed, uint16_t frequency, uint8_t time, uint8_t holdoff, uint8_t highLevel, uint8_t lowLevel, bool callsignAtStart, bool callsignAtEnd);
  uint8_t setAck(const char* rfAck, uint8_t speed, uint16_t frequency, uint8_t minTime, uint16_t delay, uint8_t level);
  uint8_t setMisc(uint16_t timeout, uint8_t timeoutLevel, uint8_t ctcssFrequency, uint8_t ctcssThreshold, uint8_t ctcssLevel, uint8_t kerchunkTime, uint8_t hangTime);

private:
  CFMKeyer             m_callsign;
  CFMKeyer             m_rfAck;
  CFMCTCSSRX           m_ctcssRX;
  CFMCTCSSTX           m_ctcssTX;
  CFMTimeout           m_timeoutTone;
  FM_STATE             m_state;
  bool                 m_callsignAtStart;
  bool                 m_callsignAtEnd;
  CFMTimer             m_callsignTimer;
  CFMTimer             m_timeoutTimer;
  CFMTimer             m_holdoffTimer;
  CFMTimer             m_kerchunkTimer;
  CFMTimer             m_ackMinTimer;
  CFMTimer             m_ackDelayTimer;
  CFMTimer             m_hangTimer;
  CFMDirectFormI       m_filterStage1;
  CFMDirectFormI       m_filterStage2;
  CFMDirectFormI       m_filterStage3;

  void stateMachine(bool validSignal, uint8_t length);
  void listeningState(bool validSignal);
  void kerchunkState(bool validSignal);
  void relayingState(bool validSignal);
  void relayingWaitState(bool validSignal);
  void timeoutState(bool validSignal);
  void timeoutWaitState(bool validSignal);
  void hangState(bool validSignal);

  void sendCallsign();
  void beginRelaying();
};

#endif
