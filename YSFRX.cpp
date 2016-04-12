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

// #define  WANT_DEBUG

#include "Config.h"
#include "Globals.h"
#include "YSFRX.h"
#include "Utils.h"

const unsigned int BUFFER_LENGTH = 200U;

const q15_t SCALING_FACTOR = 19505;      // Q15(0.60)

const uint32_t PLLMAX = 0x10000U;
const uint32_t PLLINC = PLLMAX / YSF_RADIO_SYMBOL_LENGTH;
const uint32_t INC    = PLLINC / 32U;

const uint8_t SYNC_SYMBOL_ERRS = 2U;
const uint8_t SYNC_BIT_ERRS    = 4U;

const unsigned int MAX_SYNC_FRAMES = 4U + 1U;

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])
#define READ_BIT1(p,i)    (p[(i)>>3] & BIT_MASK_TABLE[(i)&7])

CYSFRX::CYSFRX() :
m_pll(0U),
m_prev(false),
m_state(YSFRXS_NONE),
m_bitBuffer(0x00U),
m_symbols(),
m_outBuffer(),
m_buffer(NULL),
m_bufferPtr(0U),
m_symbolPtr(0U),
m_lostCount(0U),
m_centre(0),
m_threshold(0)
{
  m_buffer = m_outBuffer + 1U;
}

void CYSFRX::reset()
{
  m_pll       = 0U;
  m_prev      = false;
  m_state     = YSFRXS_NONE;
  m_bitBuffer = 0x00U;
  m_bufferPtr = 0U;
  m_symbolPtr = 0U;
  m_lostCount = 0U;
  m_centre    = 0;
  m_threshold = 0;
}

void CYSFRX::samples(const q15_t* samples, uint8_t length)
{
  for (uint16_t i = 0U; i < length; i++) {
    bool bit = samples[i] < 0;

    if (bit != m_prev) {
      if (m_pll < (PLLMAX / 2U))
        m_pll += INC;
      else
        m_pll -= INC;
    }

    m_prev = bit;

    m_pll += PLLINC;

    if (m_pll >= PLLMAX) {
      m_pll -= PLLMAX;

      if (m_state == YSFRXS_NONE)
        processNone(samples[i]);
      else
        processData(samples[i]);
    }
  }
}

void CYSFRX::processNone(q15_t sample)
{
  m_symbolBuffer <<= 1;
  if (sample < 0)
    m_symbolBuffer |= 0x01U;

  m_symbols[m_symbolPtr] = sample;

  // Fuzzy matching of the data sync bit sequence
  if (countBits32((m_symbolBuffer & YSF_SYNC_SYMBOLS_MASK) ^ YSF_SYNC_SYMBOLS) <= SYNC_SYMBOL_ERRS) {
    uint16_t ptr = m_symbolPtr + 1U;
    if (ptr >= YSF_SYNC_LENGTH_SYMBOLS)
      ptr = 0U;

    q15_t max  = -16000;
    q15_t min  =  16000;

    for (uint8_t i = 0U; i < YSF_SYNC_LENGTH_SYMBOLS; i++) {
      if (m_symbols[ptr] > max)
        max = m_symbols[ptr];
      if (m_symbols[ptr] < min)
        min = m_symbols[ptr];

      ptr++;
      if (ptr >= YSF_SYNC_LENGTH_SYMBOLS)
        ptr = 0U;
    }

    q15_t centre = (max + min) >> 1;

    q31_t v1 = (max - centre) * SCALING_FACTOR;
    q15_t threshold = q15_t(v1 >> 15);

    ptr = m_symbolPtr + 1U;
    if (ptr >= YSF_SYNC_LENGTH_SYMBOLS)
      ptr = 0U;

    for (uint8_t i = 0U; i < YSF_SYNC_LENGTH_SYMBOLS; i++) {
      q15_t sample = m_symbols[ptr] - centre;

      if (sample < -threshold) {
        m_bitBuffer <<= 2;
        m_bitBuffer |= 0x01U;
      } else if (sample < 0) {
        m_bitBuffer <<= 2;
        m_bitBuffer |= 0x00U;
      } else if (sample < threshold) {
        m_bitBuffer <<= 2;
        m_bitBuffer |= 0x02U;
      } else {
        m_bitBuffer <<= 2;
        m_bitBuffer |= 0x03U;
      }

      ptr++;
      if (ptr >= YSF_SYNC_LENGTH_SYMBOLS)
        ptr = 0U;
    }

    // Fuzzy matching of the data sync bit sequence
    if (countBits64((m_bitBuffer & YSF_SYNC_BITS_MASK) ^ YSF_SYNC_BITS) <= SYNC_BIT_ERRS) {
      DEBUG5("YSFRX: sync found in None min/max/centre/threshold", min, max, centre, threshold);
      for (uint8_t i = 0U; i < YSF_SYNC_LENGTH_BYTES; i++)
        m_buffer[i] = YSF_SYNC_BYTES[i];
      m_centre    = centre;
      m_threshold = threshold;
      m_lostCount = MAX_SYNC_FRAMES;
      m_bufferPtr = YSF_SYNC_LENGTH_BITS;
      m_state     = YSFRXS_DATA;
      io.setDecode(true);
    }
  }

  m_symbolPtr++;
  if (m_symbolPtr >= YSF_SYNC_LENGTH_SYMBOLS)
    m_symbolPtr = 0U;
}

void CYSFRX::processData(q15_t sample)
{
  sample -= m_centre;

  if (sample < -m_threshold) {
    m_bitBuffer <<= 2;
    m_bitBuffer |= 0x01U;
    WRITE_BIT1(m_buffer, m_bufferPtr, false);
    m_bufferPtr++;
    WRITE_BIT1(m_buffer, m_bufferPtr, true);
    m_bufferPtr++;
  } else if (sample < 0) {
    m_bitBuffer <<= 2;
    m_bitBuffer |= 0x00U;
    WRITE_BIT1(m_buffer, m_bufferPtr, false);
    m_bufferPtr++;
    WRITE_BIT1(m_buffer, m_bufferPtr, false);
    m_bufferPtr++;
  } else if (sample < m_threshold) {
    m_bitBuffer <<= 2;
    m_bitBuffer |= 0x02U;
    WRITE_BIT1(m_buffer, m_bufferPtr, true);
    m_bufferPtr++;
    WRITE_BIT1(m_buffer, m_bufferPtr, false);
    m_bufferPtr++;
  } else {
    m_bitBuffer <<= 2;
    m_bitBuffer |= 0x03U;
    WRITE_BIT1(m_buffer, m_bufferPtr, true);
    m_bufferPtr++;
    WRITE_BIT1(m_buffer, m_bufferPtr, true);
    m_bufferPtr++;
  }

  bool found = false;

  // Only search for a sync in the right place +-1 symbol
  if (m_bufferPtr >= (YSF_SYNC_LENGTH_BITS - 2U) && m_bufferPtr <= (YSF_SYNC_LENGTH_BITS + 2U)) {
    // Fuzzy matching of the data sync bit sequence
    if (countBits64((m_bitBuffer & YSF_SYNC_BITS_MASK) ^ YSF_SYNC_BITS) <= SYNC_BIT_ERRS) {
#if defined(WANT_DEBUG)
      if (m_bufferPtr < YSF_SYNC_LENGTH_BITS)
        DEBUG2("YSFRX: found sync in Data, early", YSF_SYNC_LENGTH_BITS - m_bufferPtr);
      else if (m_bufferPtr > YSF_SYNC_LENGTH_BITS)
        DEBUG2("YSFRX: found sync in Data, late", m_bufferPtr - YSF_SYNC_LENGTH_BITS);
      else
        DEBUG1("YSFRX: found sync in Data");
#endif
      m_lostCount = MAX_SYNC_FRAMES;
      m_bufferPtr = YSF_SYNC_LENGTH_BITS;
      found       = true;
    }
  }

  // Send a data frame to the host if the required number of bits have been received, or if a data sync has been seen
  if (m_bufferPtr == YSF_FRAME_LENGTH_BITS) {
    // We've not seen a data sync for too long, signal RXLOST and change to RX_NONE
    m_lostCount--;
    if (m_lostCount == 0U) {
      DEBUG1("YSFRX: sync timed out, lost lock");
      io.setDecode(false);

      serial.writeYSFLost();

      m_state = YSFRXS_NONE;
      return;
    } else {
      m_outBuffer[0U] = found ? 0x01U : 0x00U;

      serial.writeYSFData(m_outBuffer, YSF_FRAME_LENGTH_BYTES + 1U);

      // Start the next frame
      ::memset(m_outBuffer, 0x00U, YSF_FRAME_LENGTH_BYTES + 1U);
      m_bufferPtr = 0U;
    }
  }
}

