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

#if defined(MODE_AX25)

#include "Globals.h"
#include "AX25Demodulator.h"
#include "AX25Defines.h"

const float32_t SAMPLE_RATE = 24000.0F;
const float32_t SYMBOL_RATE = 1200.0F;

const uint16_t DELAY_LEN = 11U;

const float32_t SAMPLES_PER_SYMBOL = SAMPLE_RATE / SYMBOL_RATE;
const float32_t PLL_LIMIT          = SAMPLES_PER_SYMBOL / 2.0F;

const uint32_t LPF_FILTER_LEN = 48U;

q15_t LPF_FILTER_COEFFS[] = {
    -2,   -8,  -17,  -28,  -40,  -47,  -47,  -34,
    -5,   46,  122,  224,  354,  510,  689,  885,
  1092, 1302, 1506, 1693, 1856, 1987, 2077, 2124,
  2124, 2077, 1987, 1856, 1693, 1506, 1302, 1092,
  885,  689,  510,  354,  224,  122,   46,    -5,
  -34,  -47,  -47,  -40,  -28,  -17,   -8,    -2
};

// Lock low-pass filter taps (80Hz Bessel)
// scipy.signal:
//      b, a = bessel(4, [80.0/(1200/2)], 'lowpass')
//
const uint8_t PLL_IIR_SIZE = 5U;

const float32_t PLL_LOCK_B[] = {
    1.077063e-03,4.308253e-03,6.462379e-03,4.308253e-03,1.077063e-03};

const float32_t PLL_LOCK_A[] = {
    1.000000e+00,-2.774567e+00,2.962960e+00,-1.437990e+00,2.668296e-01};

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
m_pllJitter(0.0F),
m_pllDCD(false),
m_iirHistory(),
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

  for (uint8_t i = 0U; i < PLL_IIR_SIZE; i++)
    m_iirHistory[i] = 0.0F;
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

    float32_t adjust = m_pllBits > 16U ? 5.0F : 0.0F;
    float32_t offset = m_pllCount / float32_t(m_pllBits);
    float32_t jitter;
    ::arm_fir_f32(&m_pllFilter, &offset, &jitter, 1U);

    float32_t absOffset = adjust;
    if (offset < 0.0F)
      absOffset -= offset;
    else
      absOffset += offset;
    m_pllJitter = iir(absOffset);

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
  if (m_hdlcOnes == AX25_MAX_ONES) {
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
      case AX25_FRAME_END:
        if (m_frame.m_length >= AX25_MIN_FRAME_LENGTH) {
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

      case AX25_FRAME_ABORT:
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

bool CAX25Demodulator::isDCD()
{
  if (m_pllJitter <= (SAMPLES_PER_SYMBOL * 0.03F))
    m_pllDCD = true;
  else if (m_pllJitter >= (SAMPLES_PER_SYMBOL * 0.15F))
    m_pllDCD = false;

  return m_pllDCD;
}

float32_t CAX25Demodulator::iir(float32_t input)
{
  for (int8_t i = int8_t(PLL_IIR_SIZE) - 1; i != 0; i--)
    m_iirHistory[i] = m_iirHistory[i - 1];

  m_iirHistory[0] = input;
  for (uint8_t i = 1U; i < PLL_IIR_SIZE; i++)
    m_iirHistory[0] -= PLL_LOCK_A[i] * m_iirHistory[i];

  float32_t result = 0.0F;
  for (uint8_t i = 0U; i < PLL_IIR_SIZE; i++)
    result += PLL_LOCK_B[i] * m_iirHistory[i];

  return result;
}

#endif

