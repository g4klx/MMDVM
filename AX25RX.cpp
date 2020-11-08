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
#include "AX25RX.h"

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

const uint32_t FILTER_LEN = 130U;

q15_t FILTER_COEFFS[] = {
      5,    12,    18,    21,   19,   11,    -2,   -15,   -25,   -27,
    -21,   -11,    -3,    -5,  -19,  -43,   -69,   -83,   -73,   -35,
     27,    98,   155,   180,  163,  109,    39,   -20,   -45,   -26,
     23,    74,    89,    39,  -81, -247,  -407,  -501,  -480,  -334,
    -92,   175,   388,   479,  429,  275,    99,     5,    68,   298,
    626,   913,   994,   740,  115, -791, -1770, -2544, -2847, -2509,
  -1527,   -76,  1518,  2875, 3653, 3653,  2875,  1518,   -76, -1527,
  -2509, -2847, -2544, -1770, -791,  115,   740,   994,   913,   626,
    298,    68,     5,    99,  275,  429,   479,   388,   175,   -92,
   -334,  -480,  -501,  -407, -247,  -81,    39,    89,    74,    23,
    -26,   -45,   -20,    39,  109,  163,   180,   155,    98,    27,
    -35,   -73,   -83,   -69,  -43,  -19,    -5,    -3,   -11,   -21,
    -27,   -25,   -15,    -2,   11,   19,    21,    18,    12,     5
};

CAX25RX::CAX25RX() :
m_filter(),
m_state(),
m_demod1(3),
m_demod2(6),
m_demod3(9),
m_lastFCS(0U),
m_count(0U),
m_slotTime(30U),
m_slotCount(0U),
m_pPersist(128U),
m_dcd(false),
m_canTX(false),
m_x(1U),
m_a(0xB7U),
m_b(0x73U),
m_c(0xF6U)
{
  m_filter.numTaps = FILTER_LEN;
  m_filter.pState  = m_state;
  m_filter.pCoeffs = FILTER_COEFFS;

  initRand();
}

void CAX25RX::samples(q15_t* samples, uint8_t length)
{
  q15_t output[RX_BLOCK_SIZE];
  ::arm_fir_fast_q15(&m_filter, samples, output, RX_BLOCK_SIZE);

  m_count++;

  CAX25Frame frame;

  bool ret = m_demod1.process(output, length, frame);
  if (ret) {
    if (frame.m_fcs != m_lastFCS || m_count > 2U) {
      m_lastFCS = frame.m_fcs;
      m_count   = 0U;
      serial.writeAX25Data(frame.m_data, frame.m_length - 2U);
    }
    DEBUG1("Decoder 1 reported");
  }

  ret = m_demod2.process(output, length, frame);
  if (ret) {
    if (frame.m_fcs != m_lastFCS || m_count > 2U) {
      m_lastFCS = frame.m_fcs;
      m_count   = 0U;
      serial.writeAX25Data(frame.m_data, frame.m_length - 2U);
    }
    DEBUG1("Decoder 2 reported");
  }

  ret = m_demod3.process(output, length, frame);
  if (ret) {
    if (frame.m_fcs != m_lastFCS || m_count > 2U) {
      m_lastFCS = frame.m_fcs;
      m_count   = 0U;
      serial.writeAX25Data(frame.m_data, frame.m_length - 2U);
    }
    DEBUG1("Decoder 3 reported");
  }

  m_slotCount += RX_BLOCK_SIZE;
  if (m_slotCount >= m_slotTime) {
    m_slotCount = 0U;

    bool dcd1 = m_demod1.isDCD();
    bool dcd2 = m_demod2.isDCD();
    bool dcd3 = m_demod3.isDCD();
    
    if (dcd1 || dcd2 || dcd3) {
      if (!m_dcd) {
        io.setDecode(true);
        io.setADCDetection(true);
        m_dcd = true;
      }

      m_canTX = false;
    } else {
      if (m_dcd) {
        io.setDecode(false);
        io.setADCDetection(false);
        m_dcd = false;
      }

      m_canTX = m_pPersist >= rand();
    }
  }
}

bool CAX25RX::canTX() const
{
  return m_canTX;
}

void CAX25RX::setParams(int8_t twist, uint8_t slotTime, uint8_t pPersist)
{
  m_demod1.setTwist(twist - 3);
  m_demod2.setTwist(twist);
  m_demod3.setTwist(twist + 3);

  m_slotTime = slotTime * 240U;    // Slot time in samples
  m_pPersist = pPersist;
}

// Taken from https://www.electro-tech-online.com/threads/ultra-fast-pseudorandom-number-generator-for-8-bit.124249/
//X ABC Algorithm Random Number Generator for 8-Bit Devices:
//This is a small PRNG, experimentally verified to have at least a 50 million byte period
//by generating 50 million bytes and observing that there were no overapping sequences and repeats.
//This generator passes serial correlation, entropy , Monte Carlo Pi value, arithmetic mean,
//And many other statistical tests. This generator may have a period of up to 2^32, but this has
//not been verified.
//
// By XORing 3 bytes into the a,b, and c registers, you can add in entropy from 
//an external source easily.
//
//This generator is free to use, but is not suitable for cryptography due to its short period(by //cryptographic standards) and simple construction. No attempt was made to make this generator 
// suitable for cryptographic use.
//
//Due to the use of a constant counter, the generator should be resistant to latching up.
//A significant performance gain is had in that the x variable is only ever incremented.
//
//Only 4 bytes of ram are needed for the internal state, and generating a byte requires 3 XORs , //2 ADDs, one bit shift right , and one increment. Difficult or slow operations like multiply, etc 
//were avoided for maximum speed on ultra low power devices.


void CAX25RX::initRand() //Can also be used to seed the rng with more entropy during use.
{
  m_a = (m_a ^ m_c ^ m_x);
  m_b = (m_b + m_a);
  m_c = (m_c + (m_b >> 1) ^ m_a);
}

uint8_t CAX25RX::rand()
{
  m_x++;                           //x is incremented every round and is not affected by any other variable

  m_a = (m_a ^ m_c ^ m_x);         //note the mix of addition and XOR
  m_b = (m_b + m_a);               //And the use of very few instructions
  m_c = (m_c + (m_b >> 1) ^ m_a);  //the right shift is to ensure that high-order bits from b can affect  

  return uint8_t(m_c);             //low order bits of other variables
}

#endif

