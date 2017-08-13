/*
 *   Copyright (C) 2015,2016,2017 by Jonathan Naylor G4KLX
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

#if !defined(IO_H)
#define  IO_H

#include "Globals.h"

#include "SampleRB.h"
#include "RSSIRB.h"

class CIO {
public:
  CIO();

  void start();

  void process();

  void write(MMDVM_STATE mode, q15_t* samples, uint16_t length, const uint8_t* control = NULL);

  uint16_t getSpace() const;

  void setDecode(bool dcd);
  void setADCDetection(bool detect);
  void setMode();
  
  void interrupt();

  void setParameters(bool rxInvert, bool txInvert, bool pttInvert, uint8_t rxLevel, uint8_t cwIdTXLevel, uint8_t dstarTXLevel, uint8_t dmrTXLevel, uint8_t ysfTXLevel, uint8_t p25TXLevel);

  void getOverflow(bool& adcOverflow, bool& dacOverflow);

  bool hasTXOverflow();
  bool hasRXOverflow();

  bool hasLockout() const;

  void resetWatchdog();

private:
  bool                 m_started;

  CSampleRB            m_rxBuffer;
  CSampleRB            m_txBuffer;
  CRSSIRB              m_rssiBuffer;

  arm_fir_instance_q15 m_boxcarFilter;
  q15_t                m_boxcarState[30U];        // NoTaps + BlockSize - 1, 6 + 20 - 1 plus some spare

  bool                 m_pttInvert;
  q15_t                m_rxLevel;
  q15_t                m_cwIdTXLevel;
  q15_t                m_dstarTXLevel;
  q15_t                m_dmrTXLevel;
  q15_t                m_ysfTXLevel;
  q15_t                m_p25TXLevel;

  uint32_t             m_ledCount;
  bool                 m_ledValue;

  bool                 m_detect;

  uint16_t             m_adcOverflow;
  uint16_t             m_dacOverflow;

  volatile uint32_t    m_watchdog;

  bool                 m_lockout;

  // Hardware specific routines
  void initInt();
  void startInt();

  bool getCOSInt();

  void setLEDInt(bool on);
  void setPTTInt(bool on);
  void setCOSInt(bool on);

  void setDStarInt(bool on);
  void setDMRInt(bool on);
  void setYSFInt(bool on);
  void setP25Int(bool on);
};

#endif

