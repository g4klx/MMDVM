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
#include "Globals.h"
#include "AX25RX.h"

// XXX There are for the wrong sample rate

// 1200Hz = -9dB, 2200Hz = 0dB; 3252Hz cutoff, 3.7 gain; cosine.
float32_t dB9[] = {
        -0.00232554104135,
        -0.142858725752,
        -0.449053780255,
        -0.770264863826,
        2.77651146344,
        -0.770264863826,
        -0.449053780255,
        -0.142858725752,
        -0.00232554104135};

// 1200Hz = -6dB, 2200Hz = 0dB; 2640Hz cutoff, 2.59 gain; cosine.
float32_t dB6[] = {
        -0.0209448226653,
        -0.130107651829,
        -0.299004731072,
        -0.45336946386,
        2.0629448761,
        -0.45336946386,
        -0.299004731072,
        -0.130107651829,
        -0.0209448226653};

// 1200Hz = -3dB, 2200Hz = 0dB; 1700Hz cutoff, 1.68 gain; cosine.
float32_t dB3[] = {
        -0.0231416146776,
        -0.0833375337803,
        -0.147937602401,
        -0.197411259519,
        1.46066084756,
        -0.197411259519,
        -0.147937602401,
        -0.0833375337803,
        -0.0231416146776};

/*
 * Generated with Scipy Filter, 152 coefficients, 1100-2300Hz bandpass,
 * Hann window, starting and ending 0 value coefficients removed.
 *
 * np.array(
 *  firwin2(152,
 *      [
 *          0.0,
 *          1000.0/(sample_rate/2),
 *          1100.0/(sample_rate/2),
 *          2350.0/(sample_rate/2),
 *          2500.0/(sample_rate/2),
 *          1.0
 *      ],
 *      [0,0,1,1,0,0],
 *      antisymmetric = False,
 *      window='hann') * 32768,
 *  dtype=int)[10:-10]
 */
const uint32_t FILTER_LEN = 132U;

// XXX This is for the wrong sample rate
q15_t FILTER_COEFFS[] = {
    4,     0,    -5,   -10,   -13,   -12,    -9,    -4,    -2,    -4,   -12,   -26,
  -41,   -52,   -51,   -35,    -3,    39,    83,   117,   131,   118,    83,    36,
   -6,   -32,   -30,    -3,    36,    67,    66,    19,   -74,  -199,  -323,  -408,
 -421,  -344,  -187,    17,   218,   364,   417,   369,   247,   106,    14,    26,
  166,   407,   676,   865,   866,   605,    68,  -675, -1484, -2171, -2547, -2471,
-1895,  -882,   394,  1692,  2747,  3337,  3337,  2747,  1692,   394,  -882, -1895,
-2471, -2547, -2171, -1484,  -675,    68,   605,   866,   865,   676,   407,   166,
   26,    14,   106,   247,   369,   417,   364,   218,    17,  -187,  -344,  -421,
 -408,  -323,  -199,   -74,    19,    66,    67,    36,    -3,   -30,   -32,    -6,
   36,    83,   118,   131,   117,    83,    39,    -3,   -35,   -51,   -52,   -41,
  -26,   -12,    -4,    -2,    -4,    -9,   -12,   -13,   -10,    -5,     0,     4};

CAX25RX::CAX25RX() :
m_filter(),
m_state(),
m_demod1(dB3, 9U),
m_demod2(dB6, 9U),
m_demod3(dB9, 9U),
m_lastFCS(0U)
{
  m_filter.numTaps = FILTER_LEN;
  m_filter.pState  = m_state;
  m_filter.pCoeffs = FILTER_COEFFS;
}

void CAX25RX::samples(q15_t* samples, uint8_t length)
{
  q15_t output[RX_BLOCK_SIZE];
  ::arm_fir_fast_q15(&m_filter, samples, output, RX_BLOCK_SIZE);

  CAX25Frame frame;

  bool ret = m_demod1.process(output, length, frame);
  if (ret && frame.m_length > 10U) {
    if (m_lastFCS != frame.m_fcs) {
      DEBUG1("Decoder 1 reported");
      m_lastFCS = frame.m_fcs;
      serial.writeAX25Data(frame.m_data, frame.m_length - 2U);
    }
  }

  ret = m_demod2.process(output, length, frame);
  if (ret && frame.m_length > 10U) {
    if (m_lastFCS != frame.m_fcs) {
      DEBUG1("Decoder 2 reported");
      m_lastFCS = frame.m_fcs;
      serial.writeAX25Data(frame.m_data, frame.m_length - 2U);
    }
  }

  ret = m_demod3.process(output, length, frame);
  if (ret && frame.m_length > 10U) {
    if (m_lastFCS != frame.m_fcs) {
      DEBUG1("Decoder 3 reported");
      m_lastFCS = frame.m_fcs;
      serial.writeAX25Data(frame.m_data, frame.m_length - 2U);
    }
  }
}

