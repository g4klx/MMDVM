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

#include "Config.h"

#if defined(MODE_AX25)

#include "Globals.h"
#include "AX25TX.h"

#include "AX25Defines.h"
#include "AX25Frame.h"


const uint8_t START_FLAG[] = { AX25_FRAME_START };
const uint8_t END_FLAG[]   = { AX25_FRAME_END };

const uint8_t BIT_MASK_TABLE1[] = { 0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U };

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE1[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE1[(i)&7])
#define READ_BIT1(p,i)    (p[(i)>>3] & BIT_MASK_TABLE1[(i)&7])

const uint8_t BIT_MASK_TABLE2[] = { 0x01U, 0x02U, 0x04U, 0x08U, 0x10U, 0x20U, 0x40U, 0x80U };

#define WRITE_BIT2(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE2[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE2[(i)&7])
#define READ_BIT2(p,i)    (p[(i)>>3] & BIT_MASK_TABLE2[(i)&7])

const uint16_t AUDIO_TABLE_LEN = 120U;

const q15_t AUDIO_TABLE_DATA[] = {
  0, 214, 428, 641, 851, 1060, 1265, 1468, 1666, 1859, 2048, 2230, 2407, 2577, 2740, 2896, 3043, 3182, 3313, 3434, 3546, 3649,
  3741, 3823, 3895, 3955, 4006, 4045, 4073, 4089, 4095, 4089, 4073, 4045, 4006, 3955, 3895, 3823, 3741, 3649, 3546, 3434, 3313,
  3182, 3043, 2896, 2740, 2577, 2407, 2230, 2048, 1859, 1666, 1468, 1265, 1060, 851, 641, 428, 214, 0, -214, -428, -641, -851,
  -1060, -1265, -1468, -1666, -1859, -2047, -2230, -2407, -2577, -2740, -2896, -3043, -3182, -3313, -3434, -3546, -3649, -3741,
  -3823, -3895, -3955, -4006, -4045, -4073, -4089, -4095, -4089, -4073, -4045, -4006, -3955, -3895, -3823, -3741, -3649, -3546,
  -3434, -3313, -3182, -3043, -2896, -2740, -2577, -2407, -2230, -2047, -1859, -1666, -1468, -1265, -1060, -851, -641, -428, -214
};

CAX25TX::CAX25TX() :
m_poBuffer(),
m_poLen(0U),
m_poPtr(0U),
m_txDelay(360U),
m_tablePtr(0U),
m_nrzi(false)
{
}

void CAX25TX::process()
{
  if (m_poLen == 0U)
    return;

  if (!m_duplex) {
    if (m_poPtr == 0U) {
      bool tx = ax25RX.canTX();
      if (!tx)
        return;
    }
  }

  uint16_t space = io.getSpace();

  while (space > AX25_RADIO_SYMBOL_LENGTH) {
    bool b = READ_BIT1(m_poBuffer, m_poPtr) != 0U;
    m_poPtr++;

    writeBit(b);

    space -= AX25_RADIO_SYMBOL_LENGTH;

    if (m_poPtr >= m_poLen) {
      m_poPtr = 0U;
      m_poLen = 0U;
      return;
    }
  }
}

uint8_t CAX25TX::writeData(const uint8_t* data, uint16_t length)
{
  CAX25Frame frame(data, length);
  frame.addCRC();

  m_poLen    = 0U;
  m_poPtr    = 0U;
  m_nrzi     = false;
  m_tablePtr = 0U;

  // Add TX delay
  for (uint16_t i = 0U; i < m_txDelay; i++, m_poLen++) {
    bool preamble = NRZI(false);
    WRITE_BIT1(m_poBuffer, m_poLen, preamble);
  }

  // Add the Start Flag
  for (uint16_t i = 0U; i < 8U; i++, m_poLen++) {
    bool b1 = READ_BIT1(START_FLAG, i) != 0U;
    bool b2 = NRZI(b1);
    WRITE_BIT1(m_poBuffer, m_poLen, b2);
  }

  uint8_t ones = 0U;
  for (uint16_t i = 0U; i < (frame.m_length * 8U); i++) {
    bool b1 = READ_BIT2(frame.m_data, i) != 0U;
    bool b2 = NRZI(b1);
    WRITE_BIT1(m_poBuffer, m_poLen, b2);
    m_poLen++;

    if (b1) {
      ones++;
      if (ones == AX25_MAX_ONES) {
        // Bit stuffing
        bool b = NRZI(false);
        WRITE_BIT1(m_poBuffer, m_poLen, b);
        m_poLen++;
        ones = 0U;
      }
    } else {
      ones = 0U;
    }
  }

  // Add the End Flag
  for (uint16_t i = 0U; i < 8U; i++, m_poLen++) {
    bool b1 = READ_BIT1(END_FLAG, i) != 0U;
    bool b2 = NRZI(b1);
    WRITE_BIT1(m_poBuffer, m_poLen, b2);
  }

  return 0U;
}

void CAX25TX::writeBit(bool b)
{
  q15_t buffer[AX25_RADIO_SYMBOL_LENGTH];
  for (uint8_t i = 0U; i < AX25_RADIO_SYMBOL_LENGTH; i++) {
    q15_t value = AUDIO_TABLE_DATA[m_tablePtr];

    if (b) {
      // De-emphasise the lower frequency by 6dB
      value >>= 2;
      m_tablePtr += 6U;
    } else {
      m_tablePtr += 11U;
    }

    buffer[i] = value >> 1;

    if (m_tablePtr >= AUDIO_TABLE_LEN)
      m_tablePtr -= AUDIO_TABLE_LEN;
  }

  io.write(STATE_AX25, buffer, AX25_RADIO_SYMBOL_LENGTH);
}

void CAX25TX::setTXDelay(uint8_t delay)
{
  m_txDelay = delay * 12U;
}

uint8_t CAX25TX::getSpace() const
{
  return m_poLen == 0U ? 255U : 0U;
}

bool CAX25TX::NRZI(bool b)
{
    if (!b)
      m_nrzi = !m_nrzi;

    return m_nrzi;
}

#endif

