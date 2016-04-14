/*
 *   Copyright (C) 2009-2016 by Jonathan Naylor G4KLX
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
#include "CalDStarTX.h"

const uint8_t HEADER[] = {0x00U, 0x00U, 0x00U, 'D', 'I', 'R', 'E', 'C', 'T', ' ', ' ',
                                               'D', 'I', 'R', 'E', 'C', 'T', ' ', ' ',
                                               'C', 'Q', 'C', 'Q', 'C', 'Q', ' ', ' ',
                                               'M', 'M', 'D', 'V', 'M', ' ', ' ', ' ',
                                               'T', 'E', 'S', 'T', 0xA9U, 0x6AU};

const uint8_t SLOW_DATA_TEXT[] = {'M', 'M', 'D', 'V', 'M', ' ', 'M', 'o', 'd', 'e', 'm', ' ', 'T', 'e', 's', 't', ' ', ' ', ' ', ' '};

CCalDStarTX::CCalDStarTX() :
m_transmit(false),
m_count(0U)
{
}

void CCalDStarTX::process()
{
  dstarTX.process();

  if (!m_transmit)
    return;

  uint16_t space = dstarTX.getSpace();
  if (space < 5U)
    return;

  uint8_t buffer[DSTAR_DATA_LENGTH_BYTES];
  for (uint8_t i = 0U; i < DSTAR_DATA_LENGTH_BYTES; i++)
    buffer[i] = DSTAR_DATA_SYNC_BYTES[i];

  if ((m_count % 21U) == 0U) {
    // Sync is already included
  } else if (m_count == 1U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ (DSTAR_SLOW_DATA_TYPE_TEXT | 0U);
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ SLOW_DATA_TEXT[0U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ SLOW_DATA_TEXT[1U];
  } else if (m_count == 2U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ SLOW_DATA_TEXT[2U];
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ SLOW_DATA_TEXT[3U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ SLOW_DATA_TEXT[4U];
  } else if (m_count == 3U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ (DSTAR_SLOW_DATA_TYPE_TEXT | 1U);
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ SLOW_DATA_TEXT[5U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ SLOW_DATA_TEXT[6U];
  } else if (m_count == 4U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ SLOW_DATA_TEXT[7U];
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ SLOW_DATA_TEXT[8U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ SLOW_DATA_TEXT[9U];
  } else if (m_count == 5U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ (DSTAR_SLOW_DATA_TYPE_TEXT | 2U);
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ SLOW_DATA_TEXT[10U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ SLOW_DATA_TEXT[11U];
  } else if (m_count == 6U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ SLOW_DATA_TEXT[12U];
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ SLOW_DATA_TEXT[13U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ SLOW_DATA_TEXT[14U];
  } else if (m_count == 7U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ (DSTAR_SLOW_DATA_TYPE_TEXT | 3U);
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ SLOW_DATA_TEXT[15U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ SLOW_DATA_TEXT[16U];
  } else if (m_count == 8U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ SLOW_DATA_TEXT[17U];
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ SLOW_DATA_TEXT[18U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ SLOW_DATA_TEXT[19U];
  } else if (m_count != 1U && (m_count % 21U) == 1U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ (DSTAR_SLOW_DATA_TYPE_HEADER | 5U);
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ HEADER[0U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ HEADER[1U];
  } else if (m_count != 2U && (m_count % 21U) == 2U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ HEADER[2U];
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ HEADER[3U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ HEADER[4U];
  } else if (m_count != 3U && (m_count % 21U) == 3U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ (DSTAR_SLOW_DATA_TYPE_HEADER | 5U);
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ HEADER[5U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ HEADER[6U];
  } else if (m_count != 4U && (m_count % 21U) == 4U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ HEADER[7U];
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ HEADER[8U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ HEADER[9U];
  } else if (m_count != 5U && (m_count % 21U) == 5U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ (DSTAR_SLOW_DATA_TYPE_HEADER | 5U);
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ HEADER[10U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ HEADER[11U];
  } else if (m_count != 6U && (m_count % 21U) == 6U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ HEADER[12U];
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ HEADER[13U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ HEADER[14U];
  } else if (m_count != 7U && (m_count % 21U) == 7U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ (DSTAR_SLOW_DATA_TYPE_HEADER | 5U);
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ HEADER[15U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ HEADER[16U];
  } else if (m_count != 8U && (m_count % 21U) == 8U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ HEADER[17U];
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ HEADER[18U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ HEADER[19U];
  } else if (m_count != 9U && (m_count % 21U) == 9U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ (DSTAR_SLOW_DATA_TYPE_HEADER | 5U);
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ HEADER[20U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ HEADER[21U];
  } else if (m_count != 10U && (m_count % 21U) == 10U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ HEADER[22U];
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ HEADER[23U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ HEADER[24U];
  } else if (m_count != 11U && (m_count % 21U) == 11U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ (DSTAR_SLOW_DATA_TYPE_HEADER | 5U);
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ HEADER[25U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ HEADER[26U];
  } else if (m_count != 12U && (m_count % 21U) == 12U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ HEADER[27U];
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ HEADER[28U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ HEADER[29U];
  } else if (m_count != 13U && (m_count % 21U) == 13U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ (DSTAR_SLOW_DATA_TYPE_HEADER | 5U);
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ HEADER[30U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ HEADER[31U];
  } else if (m_count != 14U && (m_count % 21U) == 14U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ HEADER[32U];
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ HEADER[33U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ HEADER[34U];
  } else if (m_count != 15U && (m_count % 21U) == 15U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ (DSTAR_SLOW_DATA_TYPE_HEADER | 5U);
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ HEADER[35U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ HEADER[36U];
  } else if (m_count != 16U && (m_count % 21U) == 16U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ HEADER[37U];
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ HEADER[38U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ HEADER[39U];
  } else if (m_count != 17U && (m_count % 21U) == 17U) {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ (DSTAR_SLOW_DATA_TYPE_HEADER | 1U);
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ HEADER[40U];
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ 'f';
  } else {
    buffer[9U]  = DSTAR_SCRAMBLER_BYTES[0U] ^ 'f';
    buffer[10U] = DSTAR_SCRAMBLER_BYTES[1U] ^ 'f';
    buffer[11U] = DSTAR_SCRAMBLER_BYTES[2U] ^ 'f';
  }

  dstarTX.writeData(buffer, DSTAR_DATA_LENGTH_BYTES);

  m_count = (m_count + 1U) % (30U * 21U);
}

uint8_t CCalDStarTX::write(const uint8_t* data, uint8_t length)
{
  if (length != 1U)
    return 4U;

  bool transmit = data[0U] == 1U;

  if (transmit && !m_transmit) {
    m_count = 0U;
    dstarTX.writeHeader(HEADER, DSTAR_HEADER_LENGTH_BYTES);
  } else if (!transmit && m_transmit) {
    dstarTX.writeEOT();
  }

  m_transmit = transmit;

  return 0U;
}

