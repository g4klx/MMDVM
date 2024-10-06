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
#include "AX25Twist.h"

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

q15_t* coeffs[] = {
  dB12,
  dB11,
  dB10,
  dB9,
  dB8,
  dB7,
  dB6,
  dB5,
  dB4,
  dB3,
  dB2,
  dB1,
  dB0,
  dB_1,
  dB_2,
  dB_3,
  dB_4,
  dB_5,
  dB_6
};

CAX25Twist::CAX25Twist(int8_t n) :
m_filter(),
m_state()
{
  setTwist(n);
}

void CAX25Twist::process(q15_t* in, q15_t* out, uint8_t length)
{
  ::arm_fir_fast_q15(&m_filter, in, out, length);
}

void CAX25Twist::setTwist(int8_t n)
{
  uint8_t twist = uint8_t(n + 6);

  m_filter.numTaps = 9;
  m_filter.pState  = m_state;
  m_filter.pCoeffs = coeffs[twist];
}

#endif

