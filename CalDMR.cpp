/*
 *   Copyright (C) 2009-2015 by Jonathan Naylor G4KLX
 *   Copyright (C) 2016 by Colin Durbridge G4EML
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
#include "CalDMR.h"

const uint8_t DATALC_1K[] = {0x00U,
         0x00U, 0x20U, 0x08U, 0x08U, 0x02U, 0x38U, 0x15U, 0x00U, 0x2CU, 0xA0U, 0x14U,
         0x60U, 0x84U, 0x6DU, 0xFFU, 0x57U, 0xD7U, 0x5DU, 0xF5U, 0xDEU, 0x30U, 0x30U,
         0x01U, 0x10U, 0x01U, 0x40U, 0x03U, 0xC0U, 0x13U, 0xC1U, 0x1EU, 0x80U, 0x6FU};

const uint8_t DATATERMLC_1K[] = {0x00U,
         0x00U, 0x4FU, 0x08U, 0xDCU, 0x02U, 0x88U, 0x15U, 0x78U, 0x2CU, 0xD0U, 0x14U,
         0xC0U, 0x84U, 0xADU, 0xFFU, 0x57U, 0xD7U, 0x5DU, 0xF5U, 0xD9U, 0x65U, 0x24U,
         0x02U, 0x28U, 0x06U, 0x20U, 0x0FU, 0x80U, 0x1BU, 0xC1U, 0x07U, 0x80U, 0x5CU};

const uint8_t VOICE0_1K[] = {0x00U,
         0xCEU, 0xA8U, 0xFEU, 0x83U, 0xACU, 0xC4U, 0x58U, 0x20U, 0x0AU, 0xCEU, 0xA8U,
         0xFEU, 0x83U, 0xA7U, 0x55U, 0xFDU, 0x7DU, 0xF7U, 0x5FU, 0x7CU, 0xC4U, 0x58U, 
         0x20U, 0x0AU, 0xCEU, 0xA8U, 0xFEU, 0x83U, 0xACU, 0xC4U, 0x58U, 0x20U, 0x0AU};

//0xA1U, 0x30U, 0x00U, 0x00U, 0x90U, 0x09U, 0x1CU,
const uint8_t VOICE1_1K[] = {0x00U,
         0xCEU, 0xA8U, 0xFEU, 0x83U, 0xACU, 0xC4U, 0x58U, 0x20U, 0x0AU, 0xCEU, 0xA8U,
         0xFEU, 0x83U, 0xA1U, 0x30U, 0x00U, 0x00U, 0x90U, 0x09U, 0x1CU, 0xC4U, 0x58U,
         0x20U, 0x0AU, 0xCEU, 0xA8U, 0xFEU, 0x83U, 0xACU, 0xC4U, 0x58U, 0x20U, 0x0AU};

//0xA1U, 0x70U, 0x00U, 0x90U, 0x00U, 0x07U, 0x4CU,
const uint8_t VOICE2_1K[] = {0x00U,
         0xCEU, 0xA8U, 0xFEU, 0x83U, 0xACU, 0xC4U, 0x58U, 0x20U, 0x0AU, 0xCEU, 0xA8U,
         0xFEU, 0x83U, 0xA1U, 0x70U, 0x00U, 0x90U, 0x00U, 0x07U, 0x4CU, 0xC4U, 0x58U,
         0x20U, 0x0AU, 0xCEU, 0xA8U, 0xFEU, 0x83U, 0xACU, 0xC4U, 0x58U, 0x20U, 0x0AU};

//0xA1U, 0x70U, 0x00U, 0x31U, 0x40U, 0x07U, 0x4CU,
const uint8_t VOICE3_1K[] = {0x00U,
         0xCEU, 0xA8U, 0xFEU, 0x83U, 0xACU, 0xC4U, 0x58U, 0x20U, 0x0AU, 0xCEU, 0xA8U,
         0xFEU, 0x83U, 0xA1U, 0x70U, 0x00U, 0x31U, 0x40U, 0x07U, 0x4CU, 0xC4U, 0x58U,
         0x20U, 0x0AU, 0xCEU, 0xA8U, 0xFEU, 0x83U, 0xACU, 0xC4U, 0x58U, 0x20U, 0x0AU};

//0xA1U, 0x50U, 0xA1U, 0x71U, 0xD1U, 0x70U, 0x7CU,
const uint8_t VOICE4_1K[] = {0x00U,
         0xCEU, 0xA8U, 0xFEU, 0x83U, 0xACU, 0xC4U, 0x58U, 0x20U, 0x0AU, 0xCEU, 0xA8U,
         0xFEU, 0x83U, 0xA1U, 0x50U, 0xA1U, 0x71U, 0xD1U, 0x70U, 0x7CU, 0xC4U, 0x58U,
         0x20U, 0x0AU, 0xCEU, 0xA8U, 0xFEU, 0x83U, 0xACU, 0xC4U, 0x58U, 0x20U, 0x0AU};

//0xA1U, 0x10U, 0x00U, 0x00U, 0x00U, 0x0EU, 0x2CU,
const uint8_t VOICE5_1K[] = {0x00U,
         0xCEU, 0xA8U, 0xFEU, 0x83U, 0xACU, 0xC4U, 0x58U, 0x20U, 0x0AU, 0xCEU, 0xA8U,
         0xFEU, 0x83U, 0xA1U, 0x10U, 0x00U, 0x00U, 0x00U, 0x0EU, 0x2CU, 0xC4U, 0x58U,
         0x20U, 0x0AU, 0xCEU, 0xA8U, 0xFEU, 0x83U, 0xACU, 0xC4U, 0x58U, 0x20U, 0x0AU};

const uint8_t SHORTLC_1K[] = {0x33U, 0x3AU, 0xA0U, 0x30U, 0x00U, 0x55U, 0xA6U, 0x5FU, 0x50U};

CCalDMR::CCalDMR() :
m_transmit(false),
m_state(DMR1KCAL_IDLE),
m_frame_start(0U)
{
}

void CCalDMR::process()
{
  switch (m_modemState) {
    case STATE_DMRCAL:
    case STATE_LFCAL:
      if (m_transmit) {
        dmrTX.setCal(true);
        dmrTX.process();
      } else {
        dmrTX.setCal(false);
      }
      break;
    case STATE_DMR1KCAL:
      dmr1kcal();
      break;
    default:
      break;
  }
}

void CCalDMR::dmr1kcal()
{
  switch (m_state) {
    case DMR1KCAL_IDLE:
      dmrTX.setStart(false);
      dmrTX.resetFifo2();
      m_state = DMR1KCAL_IDLE;
      break;
    case DMR1KCAL_DATALC:
      dmrTX.setColorCode(1U);
      dmrTX.writeShortLC(SHORTLC_1K, 9U);
      dmrTX.setStart(true);
      if (dmrTX.writeData2(DATALC_1K, DMR_FRAME_LENGTH_BYTES + 1U) == 5U)
        m_state = DMR1KCAL_DATALC;
      else
        m_state = DMR1KCAL_V0;
      break;
    case DMR1KCAL_V0:
      if (dmrTX.writeData2(VOICE0_1K, DMR_FRAME_LENGTH_BYTES + 1U) == 5U)
        m_state = DMR1KCAL_V0;
      else
        m_state = DMR1KCAL_V1;
      break;
    case DMR1KCAL_V1:
      if (dmrTX.writeData2(VOICE1_1K, DMR_FRAME_LENGTH_BYTES + 1U) == 5U)
        m_state = DMR1KCAL_V1;
      else
        m_state = DMR1KCAL_V2;
      break;
    case DMR1KCAL_V2:
      if (dmrTX.writeData2(VOICE2_1K, DMR_FRAME_LENGTH_BYTES + 1U) == 5U)
        m_state = DMR1KCAL_V2;
      else
        m_state = DMR1KCAL_V3;
      break;
    case DMR1KCAL_V3:
      if (dmrTX.writeData2(VOICE3_1K, DMR_FRAME_LENGTH_BYTES + 1U) == 5U)
        m_state = DMR1KCAL_V3;
      else
        m_state = DMR1KCAL_V4;
      break;
    case DMR1KCAL_V4:
      if (dmrTX.writeData2(VOICE4_1K, DMR_FRAME_LENGTH_BYTES + 1U) == 5U)
        m_state = DMR1KCAL_V4;
      else
        m_state = DMR1KCAL_V5;
      break;
    case DMR1KCAL_V5:
      if (dmrTX.writeData2(VOICE5_1K, DMR_FRAME_LENGTH_BYTES + 1U) == 5U)
        m_state = DMR1KCAL_V5;
      else {
        if(m_transmit)
          m_state = DMR1KCAL_V0;
        else
          m_state = DMR1KCAL_TERMLC;
      }
      break;
    case DMR1KCAL_TERMLC:
      if (dmrTX.writeData2(DATATERMLC_1K, DMR_FRAME_LENGTH_BYTES + 1U) == 5U)
        m_state = DMR1KCAL_TERMLC;
      else {
        m_state = DMR1KCAL_WAIT;
        m_frame_start = dmrTX.getFrameCount();
      }
      break;
    case DMR1KCAL_WAIT:
      if (dmrTX.getFrameCount() > (m_frame_start + 30U))
        m_state = DMR1KCAL_IDLE;
      break;
    default:
        m_state = DMR1KCAL_IDLE;
      break;
  }
}

uint8_t CCalDMR::write(const uint8_t* data, uint8_t length)
{
  if (length != 1U)
    return 4U;

  m_transmit = data[0U] == 1U;

  if(m_transmit && m_state == DMR1KCAL_IDLE && m_modemState == STATE_DMR1KCAL)
    m_state = DMR1KCAL_DATALC;

  return 0U;
}

