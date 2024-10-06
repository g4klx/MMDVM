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

#if !defined(GLOBALS_H)
#define  GLOBALS_H

#if defined(STM32F4XX)
#include "stm32f4xx.h"
#elif defined(STM32F7XX)
#include "stm32f7xx.h"
#else
#include <Arduino.h>
#undef PI //Undefine PI to get rid of annoying warning as it is also defined in arm_math.h.
#endif

#if defined(__SAM3X8E__)
#define  ARM_MATH_CM3
#elif defined(STM32F7XX)
#define  ARM_MATH_CM7
#elif defined(STM32F4XX) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define  ARM_MATH_CM4
#else
#error "Unknown processor type"
#endif

#include <arm_math.h>

enum MMDVM_STATE {
  STATE_IDLE      = 0,
  STATE_DSTAR     = 1,
  STATE_DMR       = 2,
  STATE_YSF       = 3,
  STATE_P25       = 4,
  STATE_NXDN      = 5,
  STATE_POCSAG    = 6,
  STATE_M17       = 7,
  STATE_FM        = 10,
  STATE_AX25      = 11,

  // Dummy states start at 90
  STATE_NXDNCAL1K = 91,
  STATE_DMRDMO1K  = 92,
  STATE_P25CAL1K  = 93,
  STATE_DMRCAL1K  = 94,
  STATE_LFCAL     = 95,
  STATE_RSSICAL   = 96,
  STATE_CWID      = 97,
  STATE_DMRCAL    = 98,
  STATE_DSTARCAL  = 99,
  STATE_INTCAL    = 100,
  STATE_POCSAGCAL = 101,
  STATE_FMCAL10K  = 102,
  STATE_FMCAL12K  = 103,
  STATE_FMCAL15K  = 104,
  STATE_FMCAL20K  = 105,
  STATE_FMCAL25K  = 106,
  STATE_FMCAL30K  = 107,
  STATE_M17CAL    = 108
};

#include "SerialPort.h"
#include "DMRIdleRX.h"
#include "DMRDMORX.h"
#include "DMRDMOTX.h"
#include "DStarRX.h"
#include "DStarTX.h"
#include "DMRRX.h"
#include "DMRTX.h"
#include "YSFRX.h"
#include "YSFTX.h"
#include "P25RX.h"
#include "P25TX.h"
#include "M17RX.h"
#include "M17TX.h"
#include "NXDNRX.h"
#include "NXDNTX.h"
#include "POCSAGTX.h"
#include "CalDStarRX.h"
#include "CalDStarTX.h"
#include "CalFM.h"
#include "CalDMR.h"
#include "CalP25.h"
#include "CalNXDN.h"
#include "CalPOCSAG.h"
#include "CalRSSI.h"
#include "CWIdTX.h"
#include "AX25RX.h"
#include "AX25TX.h"
#include "CalM17.h"
#include "Debug.h"
#include "IO.h"
#include "FM.h"

const uint8_t  MARK_SLOT1 = 0x08U;
const uint8_t  MARK_SLOT2 = 0x04U;
const uint8_t  MARK_NONE  = 0x00U;

const uint16_t RX_BLOCK_SIZE = 2U;

const uint16_t TX_RINGBUFFER_SIZE = 500U;
const uint16_t RX_RINGBUFFER_SIZE = 600U;

#if defined(__MK20DX256__)
const uint16_t TX_BUFFER_LEN = 2000U;
#else
const uint16_t TX_BUFFER_LEN = 4000U;
#endif

extern MMDVM_STATE m_modemState;

extern bool m_dstarEnable;
extern bool m_dmrEnable;
extern bool m_ysfEnable;
extern bool m_p25Enable;
extern bool m_nxdnEnable;
extern bool m_pocsagEnable;
extern bool m_m17Enable;
extern bool m_fmEnable;
extern bool m_ax25Enable;

extern bool m_duplex;

extern bool m_tx;
extern bool m_dcd;

extern CSerialPort serial;
extern CIO io;

#if defined(MODE_DSTAR)
extern CDStarRX dstarRX;
extern CDStarTX dstarTX;

extern CCalDStarRX calDStarRX;
extern CCalDStarTX calDStarTX;
#endif

#if defined(MODE_DMR)
extern CDMRIdleRX dmrIdleRX;
extern CDMRRX dmrRX;
extern CDMRTX dmrTX;

extern CDMRDMORX dmrDMORX;
extern CDMRDMOTX dmrDMOTX;

extern CCalDMR calDMR;
#endif

#if defined(MODE_YSF)
extern CYSFRX ysfRX;
extern CYSFTX ysfTX;
#endif

#if defined(MODE_P25)
extern CP25RX p25RX;
extern CP25TX p25TX;

extern CCalP25 calP25;
#endif

#if defined(MODE_NXDN)
extern CNXDNRX nxdnRX;
extern CNXDNTX nxdnTX;

extern CCalNXDN calNXDN;
#endif

#if defined(MODE_POCSAG)
extern CPOCSAGTX  pocsagTX;
extern CCalPOCSAG calPOCSAG;
#endif

#if defined(MODE_M17)
extern CM17RX m17RX;
extern CM17TX m17TX;

extern CCalM17 calM17;
#endif

#if defined(MODE_FM)
extern CFM    fm;
extern CCalFM calFM;
#endif

#if defined(MODE_AX25)
extern CAX25RX ax25RX;
extern CAX25TX ax25TX;
#endif

extern CCalRSSI calRSSI;

extern CCWIdTX cwIdTX;

#endif

