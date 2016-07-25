/*
 *   Copyright (C) 2015,2016 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#if !defined(IO_H)
#define  IO_H

#include "Globals.h"

#include "SampleRB.h"

class CIO {
public:
  CIO();

  void start();

  void process();

  void write(q15_t* samples, uint16_t length, const uint8_t* control = NULL);

  uint16_t getSpace() const;

  void setDecode(bool dcd);
  void setADCDetection(bool detect);
  void setMode();
  
  void interrupt();

  void setParameters(bool rxInvert, bool txInvert, bool pttInvert, uint8_t rxLevel, uint8_t txLevel);

  void getOverflow(bool& adcOverflow, bool& dacOverflow);

  bool hasTXOverflow();
  bool hasRXOverflow();

  bool hasLockout() const;

  void resetWatchdog();

private:
#if defined(__MBED__)
  DigitalOut           m_pinPTT;
  DigitalOut           m_pinCOSLED;
  DigitalOut           m_pinLED;
  DigitalIn            m_pinCOS;

  AnalogIn             m_pinADC;
  AnalogOut            m_pinDAC;

  Ticker               m_ticker;
#endif

  bool                 m_started;

  CSampleRB            m_rxBuffer;
  CSampleRB            m_txBuffer;

  arm_fir_instance_q15 m_C4FSKFilter;
  arm_fir_instance_q15 m_GMSKFilter;
  q15_t                m_C4FSKState[70U];    // NoTaps + BlockSize - 1, 42 + 20 - 1 plus some spare
  q15_t                m_GMSKState[40U];     // NoTaps + BlockSize - 1, 12 + 20 - 1 plus some spare

  bool                 m_pttInvert;
  q15_t                m_rxLevel;
  q15_t                m_txLevel;

  uint32_t             m_ledCount;
  bool                 m_ledValue;

  bool                 m_dcd;
  bool                 m_detect;

  uint16_t             m_adcOverflow;
  uint16_t             m_dacOverflow;

  uint32_t             m_count;

  volatile uint32_t    m_watchdog;

  bool                 m_lockout;
};

#endif

