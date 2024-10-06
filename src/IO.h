/*
 *   Copyright (C) 2015,2016,2017,2018,2020,2021 by Jonathan Naylor G4KLX
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

#include "RingBuffer.h"

struct TSample {
  volatile uint16_t sample;
  volatile uint8_t control;
};

class CIO {
public:
  CIO();

  void start();

  void process();

  void write(MMDVM_STATE mode, q15_t* samples, uint16_t length, const uint8_t* control = NULL);

  uint16_t getSpace() const;

  void setDecode(bool dcd);
  void setADCDetection(bool detect);
  void setMode(MMDVM_STATE state);
  
  void interrupt();

  void setParameters(bool rxInvert, bool txInvert, bool pttInvert, uint8_t rxLevel, uint8_t cwIdTXLevel, uint8_t dstarTXLevel, uint8_t dmrTXLevel, uint8_t ysfTXLevel, uint8_t p25TXLevel, uint8_t nxdnTXLevel, uint8_t m17TXLevel, uint8_t pocsagTXLevel, uint8_t fmTXLevel, uint8_t ax25TXLevel, int16_t txDCOffset, int16_t rxDCOffset, bool useCOSAsLockout);

  void getOverflow(bool& adcOverflow, bool& dacOverflow);

  bool hasTXOverflow();
  bool hasRXOverflow();

  bool hasLockout() const;

  void resetWatchdog();
  uint32_t getWatchdog();
  
  uint8_t getCPU() const;

  void getUDID(uint8_t* buffer);

  void selfTest();

private:
  bool                  m_started;

  CRingBuffer<TSample>  m_rxBuffer;
  CRingBuffer<TSample>  m_txBuffer;
  CRingBuffer<uint16_t> m_rssiBuffer;

#if defined(USE_DCBLOCKER)
  arm_biquad_casd_df1_inst_q31 m_dcFilter;
  q31_t                        m_dcState[4];
#endif

#if defined(MODE_DSTAR)
  arm_fir_instance_q15 m_gaussianFilter;
  q15_t                m_gaussianState[40U];      // NoTaps + BlockSize - 1, 12 + 20 - 1 plus some spare
#endif

#if defined(MODE_DMR)
  arm_fir_instance_q15 m_rrc02Filter1;
  q15_t                m_rrc02State1[70U];         // NoTaps + BlockSize - 1, 42 + 20 - 1 plus some spare
#endif

#if defined(MODE_YSF)
  arm_fir_instance_q15 m_rrc02Filter2;
  q15_t                m_rrc02State2[70U];         // NoTaps + BlockSize - 1, 42 + 20 - 1 plus some spare
#endif

#if defined(MODE_P25)
  arm_fir_instance_q15 m_boxcar5Filter;
  q15_t                m_boxcar5State[30U];        // NoTaps + BlockSize - 1,  6 + 20 - 1 plus some spare
#endif

#if defined(MODE_NXDN)
#if defined(USE_NXDN_BOXCAR)
  arm_fir_instance_q15 m_boxcar10Filter;
  q15_t                m_boxcar10State[40U];      // NoTaps + BlockSize - 1, 10 + 20 - 1 plus some spare
#else
  arm_fir_instance_q15 m_nxdnFilter;
  arm_fir_instance_q15 m_nxdnISincFilter;
  q15_t                m_nxdnState[110U];         // NoTaps + BlockSize - 1, 82 + 20 - 1 plus some spare
  q15_t                m_nxdnISincState[60U];     // NoTaps + BlockSize - 1, 32 + 20 - 1 plus some spare
#endif
#endif

#if defined(MODE_M17)
  arm_fir_instance_q15 m_rrc05Filter;
  q15_t                m_rrc05State[70U];         // NoTaps + BlockSize - 1, 42 + 20 - 1 plus some spare
#endif

  bool                 m_pttInvert;
  q15_t                m_rxLevel;
  q15_t                m_cwIdTXLevel;
  q15_t                m_dstarTXLevel;
  q15_t                m_dmrTXLevel;
  q15_t                m_ysfTXLevel;
  q15_t                m_p25TXLevel;
  q15_t                m_nxdnTXLevel;
  q15_t                m_m17TXLevel;
  q15_t                m_pocsagTXLevel;
  q15_t                m_fmTXLevel;
  q15_t                m_ax25TXLevel;

  uint16_t             m_rxDCOffset;
  uint16_t             m_txDCOffset;

  bool                 m_useCOSAsLockout;

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
  void setNXDNInt(bool on);
  void setPOCSAGInt(bool on);
  void setM17Int(bool on);
  void setFMInt(bool on);
  
  void delayInt(unsigned int dly);
};

#endif
