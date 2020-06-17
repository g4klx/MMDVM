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

// 1200Hz = -12dB, 2200Hz = 0dB; 3381Hz cutoff; cosine.
q15_t dB12[] = {
  176,
  -812,
  -3916,
  -7586,
  23536,
  -7586,
  -3916,
  -812,
  176
};

// 1200Hz = -11dB, 2200Hz = 0dB; 3258Hz cutoff; cosine.
q15_t dB11[] = {
  121,
  -957,
  -3959,
  -7383,
  23871,
  -7383,
  -3959,
  -957,
  121
};

// 1200Hz = -10dB, 2200Hz = 0dB; 3118Hz cutoff; cosine.
q15_t dB10[] = {
  56,
  -1110,
  -3987,
  -7141,
  24254,
  -7141,
  -3987,
  -1110,
  56
};

// 1200Hz = -9dB, 2200Hz = 0dB; 2959Hz cutoff; cosine.
q15_t dB9[] = {
  -19,
  -1268,
  -3994,
  -6856,
  24688,
  -6856,
  -3994,
  -1268,
  -19
};

// 1200Hz = -8dB, 2200Hz = 0dB; 2778Hz cutoff; cosine.
q15_t dB8[] = {
  -104,
  -1424,
  -3968,
  -6516,
  25182,
  -6516,
  -3968,
  -1424,
  -104
};

// 1200Hz = -7dB, 2200Hz = 0dB; 2573Hz cutoff; cosine.
q15_t dB7[] = {
  -196,
  -1565,
  -3896,
  -6114,
  25742,
  -6114,
  -3896,
  -1565,
  -196
};

// 1200Hz = -6dB, 2200Hz = 0dB; 2343Hz cutoff; cosine.
q15_t dB6[] = {
  -288,
  -1676,
  -3761,
  -5642,
  26370,
  -5642,
  -3761,
  -1676,
  -288
};

// 1200Hz = -5dB, 2200Hz = 0dB; 2085Hz cutoff; cosine.
q15_t dB5[] = {
  -370,
  -1735,
  -3545,
  -5088,
  27075,
  -5088,
  -3545,
  -1735,
  -370
};

// 1200Hz = -4dB, 2200Hz = 0dB; 1790Hz cutoff; cosine.
q15_t dB4[] = {
  -432,
  -1715,
  -3220,
  -4427,
  27880,
  -4427,
  -3220,
  -1715,
  -432
};

// 1200Hz = -3dB, 2200Hz = 0dB; 1456Hz cutoff; cosine.
q15_t dB3[] = {
  -452,
  -1582,
  -2759,
  -3646,
  28792,
  -3646,
  -2759,
  -1582,
  -452
};

// 1200Hz = -2dB, 2200Hz = 0dB; 1070Hz cutoff; cosine.
q15_t dB2[] = {
  -408,
  -1295,
  -2123,
  -2710,
  29846,
  -2710,
  -2123,
  -1295,
  -408
};

// 1200Hz = -1dB, 2200Hz = 0dB; 605Hz cutoff; cosine.
q15_t dB1[] = {
  -268,
  -795,
  -1244,
  -1546,
  31116,
  -1546,
  -1244,
  -795,
  -268
};

q15_t dB0[] = {
  0,
  0,
  0,
  0,
  32767,
  0,
  0,
  0,
  0,
};

// 1200Hz = 0dB, 2200Hz = -1dB; 4130Hz cutoff; cosine.
q15_t dB_1[] = {
  -419,
  -177,
  3316,
  8650,
  11278,
  8650,
  3316,
  -177,
  -419
};

// 1200Hz = 0dB, 2200Hz = -2dB; 3190Hz cutoff; cosine.
q15_t dB_2[] = {
  -90,
  1033,
  3975,
  7267,
  8711,
  7267,
  3975,
  1033,
  -90
};

// 1200Hz = 0dB, 2200Hz = -3dB; 2330Hz cutoff; cosine.
q15_t dB_3[] = {
  292,
  1680,
  3752,
  5615,
  6362,
  5615,
  3752,
  1680,
  292
};

// 1200Hz = 0dB, 2200Hz = -4dB; 2657Hz cutoff; boxcar.
q15_t dB_4[] = {
  917,
  3024,
  5131,
  6684,
  7255,
  6684,
  5131,
  3024,
  917
};

// 1200Hz = 0dB, 2200Hz = -5dB; 2360Hz cutoff; boxcar.
q15_t dB_5[] = {
  1620,
  3339,
  4925,
  6042,
  6444,
  6042,
  4925,
  3339,
  1620
};

// 1200Hz = 0dB, 2200Hz = -6dB; 2067Hz cutoff; boxcar.
q15_t dB_6[] = {
  2161,
  3472,
  4605,
  5373,
  5644,
  5373,
  4605,
  3472,
  2161
};

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
const uint32_t FILTER_LEN = 132;
q15_t FILTER_COEFFS[] = {
      0,     5,    12,    18,    21,   19,   11,    -2,   -15,   -25,
    -27,   -21,   -11,    -3,    -5,  -19,  -43,   -69,   -83,   -73,
    -35,    27,    98,   155,   180,  163,  109,    39,   -20,   -45,
    -26,    23,    74,    89,    39,  -81, -247,  -407,  -501,  -480,
   -334,   -92,   175,   388,   479,  429,  275,    99,     5,    68,
    298,   626,   913,   994,   740,  115, -791, -1770, -2544, -2847,
  -2509, -1527,   -76,  1518,  2875, 3653, 3653,  2875,  1518,   -76,
  -1527, -2509, -2847, -2544, -1770, -791,  115,   740,   994,   913,
    626,   298,    68,     5,    99,  275,  429,   479,   388,   175,
    -92,  -334,  -480,  -501,  -407, -247,  -81,    39,    89,    74,
     23,   -26,   -45,   -20,    39,  109,  163,   180,   155,    98,
     27,   -35,   -73,   -83,   -69,  -43,  -19,    -5,    -3,   -11,
    -21,   -27,   -25,   -15,    -2,   11,   19,    21,    18,    12,
      5,     0
};

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

  m_lastFCS = 0U;
  CAX25Frame frame;

  bool ret = m_demod1.process(output, length, frame);
  if (ret && frame.m_fcs != m_lastFCS) {
    DEBUG1("Decoder 1 reported");
    m_lastFCS = frame.m_fcs;
    serial.writeAX25Data(frame.m_data, frame.m_length - 2U);
  }

  ret = m_demod2.process(output, length, frame);
  if (ret && frame.m_fcs != m_lastFCS) {
    DEBUG1("Decoder 2 reported");
    m_lastFCS = frame.m_fcs;
    serial.writeAX25Data(frame.m_data, frame.m_length - 2U);
  }

  ret = m_demod3.process(output, length, frame);
  if (ret && frame.m_fcs != m_lastFCS) {
    DEBUG1("Decoder 3 reported");
    m_lastFCS = frame.m_fcs;
    serial.writeAX25Data(frame.m_data, frame.m_length - 2U);
  }
}

