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

const float32_t DELAY       = 0.000448F;
const float32_t SAMPLE_RATE = 24000.0F;
const float32_t SYMBOL_RATE = 1200.0F;

const uint16_t DELAY_LEN = uint16_t((DELAY / (1.0F / SAMPLE_RATE)) + 0.5F);

const float32_t SAMPLES_PER_SYMBOL = SAMPLE_RATE / SYMBOL_RATE;
const float32_t PLL_LIMIT          = SAMPLES_PER_SYMBOL / 2.0F;

// XXX This is for the wrong sample rate
const uint32_t LPF_FILTER_LEN = 96U;

q15_t LPF_FILTER_COEFFS[] = {
    0,     1,     3,     5,     8,    11,    14,    17,    19,    20,    18,    14,
    7,    -2,   -16,   -33,   -53,   -76,  -101,  -126,  -151,  -174,  -194,  -208,
 -215,  -212,  -199,  -173,  -133,   -79,   -10,    74,   173,   287,   413,   549,
  693,   842,   993,  1142,  1287,  1423,  1547,  1656,  1747,  1817,  1865,  1889,
 1889,  1865,  1817,  1747,  1656,  1547,  1423,  1287,  1142,   993,   842,   693,
  549,   413,   287,   173,    74,   -10,   -79,  -133,  -173,  -199,  -212,  -215,
 -208,  -194,  -174,  -151,  -126,  -101,   -76,   -53,   -33,   -16,    -2,     7,
   14,    18,    20,    19,    17,    14,    11,     8,     5,     3,     1,     0,
};

// 64 Hz loop filter.
// scipy.signal:
//      loop_coeffs = firwin(9, [64.0/(1200/2)], width = None,
//          pass_zero = True, scale = True, window='hann')
//
const uint32_t PLL_FILTER_LEN = 7U;

float32_t PLL_FILTER_COEFFS[] = {3.196252e-02F, 1.204223e-01F, 2.176819e-01F, 2.598666e-01F, 2.176819e-01F, 1.204223e-01F, 3.196252e-02F};

CAX25Demodulator::CAX25Demodulator(float32_t* coeffs, uint16_t length) :
m_frame(),
m_audioFilter(),
m_audioState(),
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
  m_delayLine = new bool[2U * DELAY_LEN];

  m_audioFilter.numTaps = length;
  m_audioFilter.pState  = m_audioState;
  m_audioFilter.pCoeffs = coeffs;

  m_lpfFilter.numTaps = LPF_FILTER_LEN;
  m_lpfFilter.pState  = m_lpfState;
  m_lpfFilter.pCoeffs = LPF_FILTER_COEFFS;

  m_pllFilter.numTaps = PLL_FILTER_LEN;
  m_pllFilter.pState  = m_pllState;
  m_pllFilter.pCoeffs = PLL_FILTER_COEFFS;
}

bool CAX25Demodulator::process(const q15_t* samples, uint8_t length, CAX25Frame& frame)
{
  bool result = false;

  float32_t input[RX_BLOCK_SIZE];
  for (size_t i = 0; i < length; i++)
    input[i] = float(samples[i]);

  float32_t fa[RX_BLOCK_SIZE];
  ::arm_fir_f32(&m_audioFilter, input, fa, RX_BLOCK_SIZE);

  int16_t buffer[RX_BLOCK_SIZE];
  for (uint8_t i = 0; i < length; i++) {
      int16_t sample = int16_t(fa[i]);
      bool     level = (sample >= 0);
      bool   delayed = delay(level);
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
          ::memcpy(frame.m_data, m_frame.m_data, AX25_MAX_PACKET_LEN);
          frame.m_length = frame.m_length;
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
		
  if (input != m_pllLast or m_pllBits > 16U) {
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
  bool result = false;

  if (m_hdlcOnes == 5U) {
    if (b) {
      // flag byte
      m_hdlcFlag = true;
    } else {
      // bit stuffing...
      m_hdlcFlag = false;
      m_hdlcOnes = 0U;
      return result;
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
    switch (m_hdlcBuffer) {
      case 0x7E:
        if (m_frame.m_length > 0U) {
          result = m_frame.checkCRC();
          if (!result)
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

  return result;
}

