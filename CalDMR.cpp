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

// Voice LC BS Header, CC: 1, srcID: 1, dstID: TG9
const uint8_t VH_1K[] = {0x00U,
         0x00U, 0x20U, 0x08U, 0x08U, 0x02U, 0x38U, 0x15U, 0x00U, 0x2CU, 0xA0U, 0x14U,
         0x60U, 0x84U, 0x6DU, 0xFFU, 0x57U, 0xD7U, 0x5DU, 0xF5U, 0xDEU, 0x30U, 0x30U,
         0x01U, 0x10U, 0x01U, 0x40U, 0x03U, 0xC0U, 0x13U, 0xC1U, 0x1EU, 0x80U, 0x6FU};

// Voice Term BS with LC, CC: 1, srcID: 1, dstID: TG9
const uint8_t VT_1K[] = {0x00U,
         0x00U, 0x4FU, 0x08U, 0xDCU, 0x02U, 0x88U, 0x15U, 0x78U, 0x2CU, 0xD0U, 0x14U,
         0xC0U, 0x84U, 0xADU, 0xFFU, 0x57U, 0xD7U, 0x5DU, 0xF5U, 0xD9U, 0x65U, 0x24U,
         0x02U, 0x28U, 0x06U, 0x20U, 0x0FU, 0x80U, 0x1BU, 0xC1U, 0x07U, 0x80U, 0x5CU};

// Voice LC MS Header, CC: 1, srcID: 1, dstID: TG9
const uint8_t VH_DMO1K[] = {0x00U,
         0x00U, 0x20U, 0x08U, 0x08U, 0x02U, 0x38U, 0x15U, 0x00U, 0x2CU, 0xA0U, 0x14U,
         0x60U, 0x84U, 0x6DU, 0x5DU, 0x7FU, 0x77U, 0xFDU, 0x75U, 0x7EU, 0x30U, 0x30U,
         0x01U, 0x10U, 0x01U, 0x40U, 0x03U, 0xC0U, 0x13U, 0xC1U, 0x1EU, 0x80U, 0x6FU};

// Voice Term MS with LC, CC: 1, srcID: 1, dstID: TG9
const uint8_t VT_DMO1K[] = {0x00U,
         0x00U, 0x4FU, 0x08U, 0xDCU, 0x02U, 0x88U, 0x15U, 0x78U, 0x2CU, 0xD0U, 0x14U,
         0xC0U, 0x84U, 0xADU, 0x5DU, 0x7FU, 0x77U, 0xFDU, 0x75U, 0x79U, 0x65U, 0x24U,
         0x02U, 0x28U, 0x06U, 0x20U, 0x0FU, 0x80U, 0x1BU, 0xC1U, 0x07U, 0x80U, 0x5CU};

// Voice coding data + FEC, 1031 Hz Test Pattern
const uint8_t VOICE_1K[] = {0x00U,
         0xCEU, 0xA8U, 0xFEU, 0x83U, 0xACU, 0xC4U, 0x58U, 0x20U, 0x0AU, 0xCEU, 0xA8U,
         0xFEU, 0x83U, 0xA0U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x0CU, 0xC4U, 0x58U, 
         0x20U, 0x0AU, 0xCEU, 0xA8U, 0xFEU, 0x83U, 0xACU, 0xC4U, 0x58U, 0x20U, 0x0AU};

// Embedded LC BS: CC: 1, srcID: 1, dstID: TG9
const uint8_t SYNCEMB_1K[6][7] = {
         {0x07U, 0x55U, 0xFDU, 0x7DU, 0xF7U, 0x5FU, 0x70U},   // BS VOICE SYNC      (audio seq 0)
         {0x01U, 0x30U, 0x00U, 0x00U, 0x90U, 0x09U, 0x10U},   // EMB + Embedded LC1 (audio seq 1)
         {0x01U, 0x70U, 0x00U, 0x90U, 0x00U, 0x07U, 0x40U},   // EMB + Embedded LC2 (audio seq 2)
         {0x01U, 0x70U, 0x00U, 0x31U, 0x40U, 0x07U, 0x40U},   // EMB + Embedded LC3 (audio seq 3)
         {0x01U, 0x50U, 0xA1U, 0x71U, 0xD1U, 0x70U, 0x70U},   // EMB + Embedded LC4 (audio seq 4)
         {0x01U, 0x10U, 0x00U, 0x00U, 0x00U, 0x0EU, 0x20U}};  // EMB                (audio seq 5)

// Embedded LC MS: CC: 1, srcID: 1, dstID: TG9
const uint8_t SYNCEMB_DMO1K[6][7] = {
         {0x07U, 0xF7U, 0xD5U, 0xDDU, 0x57U, 0xDFU, 0xD0U},   // MS VOICE SYNC      (audio seq 0)
         {0x01U, 0x30U, 0x00U, 0x00U, 0x90U, 0x09U, 0x10U},   // EMB + Embedded LC1 (audio seq 1)
         {0x01U, 0x70U, 0x00U, 0x90U, 0x00U, 0x07U, 0x40U},   // EMB + Embedded LC2 (audio seq 2)
         {0x01U, 0x70U, 0x00U, 0x31U, 0x40U, 0x07U, 0x40U},   // EMB + Embedded LC3 (audio seq 3)
         {0x01U, 0x50U, 0xA1U, 0x71U, 0xD1U, 0x70U, 0x70U},   // EMB + Embedded LC4 (audio seq 4)
         {0x01U, 0x10U, 0x00U, 0x00U, 0x00U, 0x0EU, 0x20U}};  // EMB                (audio seq 5)

// Short LC:
// TS1: dstID: 0, ACTIVITY_NONE
// TS2: dstID: TG9, ACTIVITY_VOICE
const uint8_t SHORTLC_1K[] = {0x33U, 0x3AU, 0xA0U, 0x30U, 0x00U, 0x55U, 0xA6U, 0x5FU, 0x50U};

CCalDMR::CCalDMR() :
m_transmit(false),
m_state(DMRCAL1K_IDLE),
m_frame_start(0U),
m_dmr1k(),
m_audioSeq(0)
{
  ::memcpy(m_dmr1k, VOICE_1K, DMR_FRAME_LENGTH_BYTES + 1U);
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
    case STATE_DMRCAL1K:
      dmr1kcal();
      break;
    case STATE_DMRDMO1K:
      dmrdmo1k();
      break;
    default:
      break;
  }
}

void CCalDMR::createData1k(uint8_t n)
{
  for(uint8_t i = 0; i < 5U; i++)
    m_dmr1k[i + 15U] = SYNCEMB_1K[n][i + 1U];

  m_dmr1k[14U] &= 0xF0U;
  m_dmr1k[20U] &= 0x0FU;
  m_dmr1k[14U] |= SYNCEMB_1K[n][0] & 0x0FU;
  m_dmr1k[20U] |= SYNCEMB_1K[n][6] & 0xF0U;
}

void CCalDMR::createDataDMO1k(uint8_t n)
{
  for(uint8_t i = 0; i < 5U; i++)
    m_dmr1k[i + 15U] = SYNCEMB_DMO1K[n][i + 1U];

  m_dmr1k[14U] &= 0xF0U;
  m_dmr1k[20U] &= 0x0FU;
  m_dmr1k[14U] |= SYNCEMB_DMO1K[n][0] & 0x0FU;
  m_dmr1k[20U] |= SYNCEMB_DMO1K[n][6] & 0xF0U;
}

void CCalDMR::dmr1kcal()
{
  dmrTX.process();

  uint16_t space = dmrTX.getSpace2();
  if (space < 1U)
    return;

  switch (m_state) {
    case DMRCAL1K_VH:
      dmrTX.setColorCode(1U);
      dmrTX.writeShortLC(SHORTLC_1K, 9U);
      dmrTX.writeData2(VH_1K, DMR_FRAME_LENGTH_BYTES + 1U);
      dmrTX.setStart(true);
      m_state = DMRCAL1K_VOICE;
      break;
    case DMRCAL1K_VOICE:
      createData1k(m_audioSeq);
      dmrTX.writeData2(m_dmr1k, DMR_FRAME_LENGTH_BYTES + 1U);
      if(m_audioSeq == 5U) {
        m_audioSeq = 0U;
        if(!m_transmit)
          m_state = DMRCAL1K_VT;
      } else
        m_audioSeq++;
      break;
    case DMRCAL1K_VT:
      dmrTX.writeData2(VT_1K, DMR_FRAME_LENGTH_BYTES + 1U);
      m_frame_start = dmrTX.getFrameCount();
      m_state = DMRCAL1K_WAIT;
      break;
    case DMRCAL1K_WAIT:
      if (dmrTX.getFrameCount() > (m_frame_start + 30U)) {
        dmrTX.setStart(false);
        dmrTX.resetFifo2();
        m_audioSeq = 0U;
        m_state = DMRCAL1K_IDLE;
      }
      break;
    default:
        m_state = DMRCAL1K_IDLE;
      break;
  }
}

void CCalDMR::dmrdmo1k()
{
  dmrDMOTX.process();

  uint16_t space = dmrDMOTX.getSpace();
  if (space < 1U)
    return;

  switch (m_state) {
    case DMRCAL1K_VH:
      dmrDMOTX.writeData(VH_DMO1K, DMR_FRAME_LENGTH_BYTES + 1U);
      m_state = DMRCAL1K_VOICE;
      break;
    case DMRCAL1K_VOICE:
      createDataDMO1k(m_audioSeq);
      dmrDMOTX.writeData(m_dmr1k, DMR_FRAME_LENGTH_BYTES + 1U);
      if(m_audioSeq == 5U) {
        m_audioSeq = 0U;
        if(!m_transmit)
          m_state = DMRCAL1K_VT;
      } else
        m_audioSeq++;
      break;
    case DMRCAL1K_VT:
      dmrDMOTX.writeData(VT_DMO1K, DMR_FRAME_LENGTH_BYTES + 1U);
      m_state = DMRCAL1K_IDLE;
      break;
    default:
        m_state = DMRCAL1K_IDLE;
        m_audioSeq = 0U;
      break;
  }
}

uint8_t CCalDMR::write(const uint8_t* data, uint8_t length)
{
  if (length != 1U)
    return 4U;

  m_transmit = data[0U] == 1U;

  if(m_transmit && m_state == DMRCAL1K_IDLE && (m_modemState == STATE_DMRCAL1K || m_modemState == STATE_DMRDMO1K))
    m_state = DMRCAL1K_VH;

  return 0U;
}

