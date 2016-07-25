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

#if !defined(GLOBALS_H)
#define  GLOBALS_H

#if defined(__MBED__)
#include "mbed.h"
#else
#include <Arduino.h>
#endif

#if defined(__SAM3X8E__) || defined(__STM32F1__) || defined(__STM32F2__)
#define  ARM_MATH_CM3
#elif defined(__STM32F3__) || defined(__STM32F4__)
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
  STATE_DMRCAL    = 98,
  STATE_DSTARCAL  = 99
};

#include "SerialPort.h"
#include "DMRIdleRX.h"
#include "DStarRX.h"
#include "DStarTX.h"
#include "DMRRX.h"
#include "DMRTX.h"
#include "YSFRX.h"
#include "YSFTX.h"
#include "CalDStarRX.h"
#include "CalDStarTX.h"
#include "CalDMR.h"
#include "CWIdTX.h"
#include "Debug.h"
#include "IO.h"

const uint8_t  MARK_SLOT1 = 0x08U;
const uint8_t  MARK_SLOT2 = 0x04U;
const uint8_t  MARK_NONE  = 0x00U;

const uint16_t RX_BLOCK_SIZE = 2U;

const uint16_t TX_RINGBUFFER_SIZE = 500U;
const uint16_t RX_RINGBUFFER_SIZE = 600U;

extern MMDVM_STATE m_modemState;

extern bool m_dstarEnable;
extern bool m_dmrEnable;
extern bool m_ysfEnable;

extern bool m_tx;

extern uint32_t m_sampleCount;
extern bool     m_sampleInsert;

extern CSerialPort serial;
extern CIO io;

extern CDStarRX dstarRX;
extern CDStarTX dstarTX;

extern CDMRIdleRX dmrIdleRX;
extern CDMRRX dmrRX;
extern CDMRTX dmrTX;

extern CYSFRX ysfRX;
extern CYSFTX ysfTX;

extern CCalDStarRX calDStarRX;
extern CCalDStarTX calDStarTX;
extern CCalDMR     calDMR;

extern CCWIdTX cwIdTX;

#endif

