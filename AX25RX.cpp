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
 * Hann window.
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

q15_t FILTER_COEFFS[] = {
           0,     5,    12,    18,    21,    19,    11,    -2,   -15,
         -25,   -27,   -21,   -11,    -3,    -5,   -19,   -43,   -69,
         -83,   -73,   -35,    27,    98,   155,   180,   163,   109,
          39,   -20,   -45,   -26,    23,    74,    89,    39,   -81,
        -247,  -407,  -501,  -480,  -334,   -92,   175,   388,   479,
         429,   275,    99,     5,    68,   298,   626,   913,   994,
         740,   115,  -791, -1770, -2544, -2847, -2509, -1527,   -76,
        1518,  2875,  3653,  3653,  2875,  1518,   -76, -1527, -2509,
       -2847, -2544, -1770,  -791,   115,   740,   994,   913,   626,
         298,    68,     5,    99,   275,   429,   479,   388,   175,
         -92,  -334,  -480,  -501,  -407,  -247,   -81,    39,    89,
          74,    23,   -26,   -45,   -20,    39,   109,   163,   180,
         155,    98,    27,   -35,   -73,   -83,   -69,   -43,   -19,
          -5,    -3,   -11,   -21,   -27,   -25,   -15,    -2,    11,
          19,    21,    18,    12,     5,     0};

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

