/*
 *   Copyright (C) 2020 by Jonathan Naylor G4KLX
 *   Copyright 2015-2019 Mobilinkd LLC <rob@mobilinkd.com>
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
#include "AX25Demodulator.h"

const float32_t SAMPLE_RATE = 24000.0F;
const float32_t SYMBOL_RATE = 1200.0F;

const uint16_t DELAY_LEN = 11U;

const float32_t SAMPLES_PER_SYMBOL = SAMPLE_RATE / SYMBOL_RATE;
const float32_t PLL_LIMIT          = SAMPLES_PER_SYMBOL / 2.0F;

const uint32_t LPF_FILTER_LEN = 96U;

q15_t LPF_FILTER_COEFFS[] = {
     0,    0,    0,    1,    2,    5,    8,   12,   18,   23,
    29,   33,   36,   36,   33,   25,   13,   -5,  -28,  -57,
   -89, -123, -159, -194, -225, -249, -264, -267, -254, -224,
  -175, -104,  -12,  102,  236,  389,  557,  738,  926, 1117,
  1306, 1486, 1654, 1802, 1927, 2025, 2092, 2126, 2126, 2092,
  2025, 1927, 1802, 1654, 1486, 1306, 1117,  926,  738,  557,
   389,  236,  102,  -12, -104, -175, -224, -254, -267, -264,
  -249, -225, -194, -159, -123,  -89,  -57,  -28,   -5,   13,
    25,   33,   36,   36,   33,   29,   23,   18,   12,    8,
     5,    2,    1,    0,    0,    0
};

// 64 Hz loop filter.
// scipy.signal:
//      loop_coeffs = firwin(9, [64.0/(1200/2)], width = None,
//          pass_zero = True, scale = True, window='hann')
//
const uint32_t PLL_FILTER_LEN = 7U;

float32_t PLL_FILTER_COEFFS[] = {3.196252e-02F, 1.204223e-01F, 2.176819e-01F, 2.598666e-01F, 2.176819e-01F, 1.204223e-01F, 3.196252e-02F};

CAX25Demodulator::CAX25Demodulator(int8_t n) :
m_frame(),
m_twist(n),
m_lpfFilter(),
m_lpfState(),
m_delayLine(NULL),
m_delayPos(0U),
m_nrziState(false),
m_pllFilter(),
m_pllState(),
m_pllLast(false),
m_pllBits(1U),
m_pllCount(0.0F),
m_hdlcOnes(0U),
m_hdlcFlag(false),
m_hdlcBuffer(0U),
m_hdlcBits(0U),
m_hdlcState(AX25_IDLE)
{
  m_delayLine = new bool[DELAY_LEN];

  m_lpfFilter.numTaps = LPF_FILTER_LEN;
  m_lpfFilter.pState  = m_lpfState;
  m_lpfFilter.pCoeffs = LPF_FILTER_COEFFS;

  m_pllFilter.numTaps = PLL_FILTER_LEN;
  m_pllFilter.pState  = m_pllState;
  m_pllFilter.pCoeffs = PLL_FILTER_COEFFS;
}

bool CAX25Demodulator::process(q15_t* samples, uint8_t length, CAX25Frame& frame)
{
  bool result = false;

  q15_t fa[RX_BLOCK_SIZE];
  m_twist.process(samples, fa, RX_BLOCK_SIZE);

  int16_t buffer[RX_BLOCK_SIZE];
  for (uint8_t i = 0; i < length; i++) {
      bool   level = (fa[i] >= 0);
      bool delayed = delay(level);
      buffer[i] = (int16_t(level ^ delayed) << 1) - 1;
  }

  q15_t fc[RX_BLOCK_SIZE];
  ::arm_fir_fast_q15(&m_lpfFilter, buffer, fc, RX_BLOCK_SIZE);

  for (uint8_t i = 0; i < length; i++) {
    bool bit = fc[i] >= 0;
    bool sample = PLL(bit);

    if (sample) {
      // We will only ever get one frame because there are
      // not enough bits in a block for more than one.
      if (result) {
        HDLC(NRZI(bit));
      } else {
        result = HDLC(NRZI(bit));
        if (result) {
          // Copy the frame data.
          ::memcpy(frame.m_data, m_frame.m_data, AX25_MAX_PACKET_LEN);
          frame.m_length = m_frame.m_length;
          frame.m_fcs    = m_frame.m_fcs;
          m_frame.m_length = 0U;
        }
      }
    }
  }

  return result;
}

bool CAX25Demodulator::delay(bool b)
{
  bool r = m_delayLine[m_delayPos];

  m_delayLine[m_delayPos++] = b;

  if (m_delayPos >= DELAY_LEN)
    m_delayPos = 0U;

  return r;
}

bool CAX25Demodulator::NRZI(bool b)
{
  bool result = (b == m_nrziState);

  m_nrziState = b;

  return result;
}

bool CAX25Demodulator::PLL(bool input)
{
  bool sample = false;
		
  if (input != m_pllLast || m_pllBits > 16U) {
    // Record transition.
    m_pllLast = input;

    if (m_pllCount > PLL_LIMIT)
      m_pllCount -= SAMPLES_PER_SYMBOL;

    float32_t offset = m_pllCount / float32_t(m_pllBits);
    float32_t jitter;
    ::arm_fir_f32(&m_pllFilter, &offset, &jitter, 1U);

    m_pllCount -= jitter / 2.0F;
    m_pllBits = 1U;
  } else {
    if (m_pllCount > PLL_LIMIT) {
      sample = true;
      m_pllCount -= SAMPLES_PER_SYMBOL;
      m_pllBits++;
    }
  }

  m_pllCount += 1.0F;

  return sample;
}

bool CAX25Demodulator::HDLC(bool b)
{
  if (m_hdlcOnes == 5U) {
    if (b) {
      // flag byte
      m_hdlcFlag = true;
    } else {
      // bit stuffing...
      m_hdlcFlag = false;
      m_hdlcOnes = 0U;
      return false;
    }
  }

  m_hdlcBuffer >>= 1;
  m_hdlcBuffer |= b ? 128U : 0U;
  m_hdlcBits++;                      // Free-running until Sync byte.

  if (b)
    m_hdlcOnes++;
  else
    m_hdlcOnes = 0U;

  if (m_hdlcFlag) {
    bool result = false;

    switch (m_hdlcBuffer) {
      case 0x7E:
        if (m_frame.m_length >= 17U) {
          result = m_frame.checkCRC();
          if (!result)
              m_frame.m_length = 0U;
        } else {
            m_frame.m_length = 0U;
        }
        m_hdlcState = AX25_SYNC;
        m_hdlcFlag = false;
        m_hdlcBits = 0U;
        break;

      case 0xFE:
        // Frame aborted
        m_frame.m_length = 0U;
        m_hdlcState = AX25_IDLE;
        m_hdlcFlag = false;
        m_hdlcBits = 0U;
        break;

      default:
        break;
    }

    return result;
  }

  switch (m_hdlcState) {
    case AX25_IDLE:
      break;

    case AX25_SYNC:
      if (m_hdlcBits == 8U) {    // 8th bit.
        // Start of frame data.
        m_hdlcState = AX25_RECEIVE;
        m_frame.append(m_hdlcBuffer);
        m_hdlcBits = 0U;
      }
      break;

    case AX25_RECEIVE:
      if (m_hdlcBits == 8U) {    // 8th bit.
        m_frame.append(m_hdlcBuffer);
        m_hdlcBits = 0U;
      }
      break;

    default:
      break;
  }

  return false;
}

void CAX25Demodulator::setTwist(int8_t n)
{
  m_twist.setTwist(n);
}

