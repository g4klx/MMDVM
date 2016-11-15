/*
 *   Copyright (C) 2015,2016 by Jonathan Naylor G4KLX
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

#if defined(STM32F4XX) || defined(STM32F4)
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_usart.h"
#include "misc.h"
#include <string.h>
#include <stddef.h>
#else
#include <Arduino.h>
#endif

#if defined(__SAM3X8E__)
#define  ARM_MATH_CM3
#elif defined(STM32F4XX) || defined(STM32F4) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define  ARM_MATH_CM4
#else
#error "Unknown processor type"
#endif

#if defined(STM32F4XX) || defined(STM32F4)
#include "cmsis.h"
#else
#include <arm_math.h>
#endif

enum MMDVM_STATE {
  STATE_IDLE      = 0,
  STATE_DSTAR     = 1,
  STATE_DMR       = 2,
  STATE_YSF       = 3,
  STATE_P25       = 4,

  // Dummy states start at 90
  STATE_CWID      = 97,
  STATE_DMRCAL    = 98,
  STATE_DSTARCAL  = 99
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
extern bool m_p25Enable;

extern bool m_duplex;

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

extern CDMRDMORX dmrDMORX;
extern CDMRDMOTX dmrDMOTX;

extern CYSFRX ysfRX;
extern CYSFTX ysfTX;

extern CP25RX p25RX;
extern CP25TX p25TX;

extern CCalDStarRX calDStarRX;
extern CCalDStarTX calDStarTX;
extern CCalDMR     calDMR;

extern CCWIdTX cwIdTX;

#endif

