/*
 *   Copyright (C) 2015,2016,2017 by Jonathan Naylor G4KLX
 *   Copyright (C) 2015 by Jim Mclaughlin KI6ZUM
 *   Copyright (C) 2016 by Colin Durbridge G4EML
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
#include "IO.h"

// Generated using [b, a] = butter(1, 0.001) in MATLAB
static q31_t   DC_FILTER[] = {3367972, 0, 3367972, 0, 2140747704, 0}; // {b0, 0, b1, b2, -a1, -a2}
const uint32_t DC_FILTER_STAGES = 1U; // One Biquad stage

// Generated using rcosdesign(0.2, 8, 5, 'sqrt') in MATLAB
static q15_t RRC_0_2_FILTER[] = {401, 104, -340, -731, -847, -553, 112, 909, 1472, 1450, 683, -675, -2144, -3040, -2706, -770, 2667, 6995,
                                   11237, 14331, 15464, 14331, 11237, 6995, 2667, -770, -2706, -3040, -2144, -675, 683, 1450, 1472, 909, 112,
                                   -553, -847, -731, -340, 104, 401, 0};
const uint16_t RRC_0_2_FILTER_LEN = 42U;

// Generated using gaussfir(0.5, 4, 5) in MATLAB
static q15_t   GAUSSIAN_0_5_FILTER[] = {8, 104, 760, 3158, 7421, 9866, 7421, 3158, 760, 104, 8, 0};
const uint16_t GAUSSIAN_0_5_FILTER_LEN = 12U;

// One symbol boxcar filter
static q15_t   BOXCAR_FILTER[] = {12000, 12000, 12000, 12000, 12000, 0};
const uint16_t BOXCAR_FILTER_LEN = 6U;

const uint16_t DC_OFFSET = 2048U;

CIO::CIO() :
m_started(false),
m_rxBuffer(RX_RINGBUFFER_SIZE),
m_txBuffer(TX_RINGBUFFER_SIZE),
m_rssiBuffer(RX_RINGBUFFER_SIZE),
m_dcFilter(),
m_dcState(),
m_rrcFilter(),
m_gaussianFilter(),
m_boxcarFilter(),
m_rrcState(),
m_gaussianState(),
m_boxcarState(),
m_pttInvert(false),
m_rxLevel(128 * 128),
m_cwIdTXLevel(128 * 128),
m_dstarTXLevel(128 * 128),
m_dmrTXLevel(128 * 128),
m_ysfTXLevel(128 * 128),
m_p25TXLevel(128 * 128),
m_ledCount(0U),
m_ledValue(true),
m_detect(false),
m_adcOverflow(0U),
m_dacOverflow(0U),
m_watchdog(0U),
m_lockout(false)
{
  ::memset(m_rrcState,      0x00U, 70U * sizeof(q15_t));
  ::memset(m_gaussianState, 0x00U, 40U * sizeof(q15_t));
  ::memset(m_boxcarState,   0x00U, 30U * sizeof(q15_t));
  ::memset(m_dcState,       0x00U,  4U * sizeof(q31_t));

  m_dcFilter.numStages = DC_FILTER_STAGES;
  m_dcFilter.pState    = m_dcState;
  m_dcFilter.pCoeffs   = DC_FILTER;
  m_dcFilter.postShift = 0;

  m_rrcFilter.numTaps = RRC_0_2_FILTER_LEN;
  m_rrcFilter.pState  = m_rrcState;
  m_rrcFilter.pCoeffs = RRC_0_2_FILTER;

  m_gaussianFilter.numTaps = GAUSSIAN_0_5_FILTER_LEN;
  m_gaussianFilter.pState  = m_gaussianState;
  m_gaussianFilter.pCoeffs = GAUSSIAN_0_5_FILTER;

  m_boxcarFilter.numTaps = BOXCAR_FILTER_LEN;
  m_boxcarFilter.pState  = m_boxcarState;
  m_boxcarFilter.pCoeffs = BOXCAR_FILTER;
  
  initInt();
}

void CIO::start()
{
  if (m_started)
    return;

  startInt();

  m_started = true;

  setMode();
}

void CIO::process()
{
  m_ledCount++;
  if (m_started) {
    // Two seconds timeout
    if (m_watchdog >= 48000U) {
      if (m_modemState == STATE_DSTAR || m_modemState == STATE_DMR || m_modemState == STATE_YSF) {
        if (m_modemState == STATE_DMR && m_tx)
          dmrTX.setStart(false);
        m_modemState = STATE_IDLE;
        setMode();
      }

      m_watchdog = 0U;
    }

    if (m_ledCount >= 24000U) {
      m_ledCount = 0U;
      m_ledValue = !m_ledValue;
      setLEDInt(m_ledValue);
    }
  } else {
    if (m_ledCount >= 240000U) {
      m_ledCount = 0U;
      m_ledValue = !m_ledValue;
      setLEDInt(m_ledValue);
    }
    return;
  }

#if defined(USE_COS_AS_LOCKOUT)
  m_lockout = getCOSInt();
#endif

  // Switch off the transmitter if needed
  if (m_txBuffer.getData() == 0U && m_tx) {
    m_tx = false;
    setPTTInt(m_pttInvert ? true : false);
  }

  if (m_rxBuffer.getData() >= RX_BLOCK_SIZE) {
    q15_t    samples[RX_BLOCK_SIZE];
    uint8_t  control[RX_BLOCK_SIZE];
    uint16_t rssi[RX_BLOCK_SIZE];

    for (uint16_t i = 0U; i < RX_BLOCK_SIZE; i++) {
      uint16_t sample;
      m_rxBuffer.get(sample, control[i]);
      m_rssiBuffer.get(rssi[i]);

      // Detect ADC overflow
      if (m_detect && (sample == 0U || sample == 4095U))
        m_adcOverflow++;

      q15_t res1 = q15_t(sample) - DC_OFFSET;
      q31_t res2 = res1 * m_rxLevel;
      samples[i] = q15_t(__SSAT((res2 >> 15), 16));
    }

    if (m_lockout)
      return;

    q31_t dcLevel = 0;
    q31_t dcValues[RX_BLOCK_SIZE];
    q31_t q31Samples[RX_BLOCK_SIZE];

    ::arm_q15_to_q31(samples, q31Samples, RX_BLOCK_SIZE);
    ::arm_biquad_cascade_df1_q31(&m_dcFilter, q31Samples, dcValues, RX_BLOCK_SIZE);

    for (uint8_t i = 0U; i < RX_BLOCK_SIZE; i++)
      dcLevel += dcValues[i];
    dcLevel /= RX_BLOCK_SIZE;

    q15_t offset = q15_t(__SSAT((dcLevel >> 16), 16));;

    q15_t dcSamples[RX_BLOCK_SIZE];
    for (uint8_t i = 0U; i < RX_BLOCK_SIZE; i++)
      dcSamples[i] = samples[i] - offset;

    if (m_modemState == STATE_IDLE) {
      if (m_dstarEnable) {
        q15_t GMSKVals[RX_BLOCK_SIZE];
        ::arm_fir_fast_q15(&m_gaussianFilter, dcSamples, GMSKVals, RX_BLOCK_SIZE);

        dstarRX.samples(GMSKVals, rssi, RX_BLOCK_SIZE);
      }

      if (m_p25Enable) {
        q15_t P25Vals[RX_BLOCK_SIZE];
        ::arm_fir_fast_q15(&m_boxcarFilter, dcSamples, P25Vals, RX_BLOCK_SIZE);

        p25RX.samples(P25Vals, rssi, RX_BLOCK_SIZE);
      }

      // XXX YSF should use dcSamples, but DMR not
      if (m_dmrEnable || m_ysfEnable) {
        q15_t C4FSKVals[RX_BLOCK_SIZE];
        ::arm_fir_fast_q15(&m_rrcFilter, samples, C4FSKVals, RX_BLOCK_SIZE);

        if (m_dmrEnable) {
          if (m_duplex)
            dmrIdleRX.samples(C4FSKVals, RX_BLOCK_SIZE);
          else
            dmrDMORX.samples(C4FSKVals, rssi, RX_BLOCK_SIZE);
        }

        if (m_ysfEnable)
          ysfRX.samples(C4FSKVals, rssi, RX_BLOCK_SIZE);
      }
    } else if (m_modemState == STATE_DSTAR) {
      if (m_dstarEnable) {
        q15_t GMSKVals[RX_BLOCK_SIZE];
        ::arm_fir_fast_q15(&m_gaussianFilter, dcSamples, GMSKVals, RX_BLOCK_SIZE);

        dstarRX.samples(GMSKVals, rssi, RX_BLOCK_SIZE);
      }
    } else if (m_modemState == STATE_DMR) {
      if (m_dmrEnable) {
        q15_t C4FSKVals[RX_BLOCK_SIZE];
        ::arm_fir_fast_q15(&m_rrcFilter, samples, C4FSKVals, RX_BLOCK_SIZE);

        if (m_duplex) {
          // If the transmitter isn't on, use the DMR idle RX to detect the wakeup CSBKs
          if (m_tx)
            dmrRX.samples(C4FSKVals, rssi, control, RX_BLOCK_SIZE);
          else
            dmrIdleRX.samples(C4FSKVals, RX_BLOCK_SIZE);
        } else {
          dmrDMORX.samples(C4FSKVals, rssi, RX_BLOCK_SIZE);
        }
      }
    } else if (m_modemState == STATE_YSF) {
      if (m_ysfEnable) {
        q15_t C4FSKVals[RX_BLOCK_SIZE];
        ::arm_fir_fast_q15(&m_rrcFilter, dcSamples, C4FSKVals, RX_BLOCK_SIZE);

        ysfRX.samples(C4FSKVals, rssi, RX_BLOCK_SIZE);
      }
    } else if (m_modemState == STATE_P25) {
      if (m_p25Enable) {
        q15_t P25Vals[RX_BLOCK_SIZE];
        ::arm_fir_fast_q15(&m_boxcarFilter, dcSamples, P25Vals, RX_BLOCK_SIZE);

        p25RX.samples(P25Vals, rssi, RX_BLOCK_SIZE);
      }
    } else if (m_modemState == STATE_DSTARCAL) {
      q15_t GMSKVals[RX_BLOCK_SIZE];
      ::arm_fir_fast_q15(&m_gaussianFilter, samples, GMSKVals, RX_BLOCK_SIZE);

      calDStarRX.samples(GMSKVals, RX_BLOCK_SIZE);
    } else if (m_modemState == STATE_RSSICAL) {
      calRSSI.samples(rssi, RX_BLOCK_SIZE);
    }
  }
}

void CIO::write(MMDVM_STATE mode, q15_t* samples, uint16_t length, const uint8_t* control)
{
  if (!m_started)
    return;

  if (m_lockout)
    return;

  // Switch the transmitter on if needed
  if (!m_tx) {
    m_tx = true;
    setPTTInt(m_pttInvert ? false : true);
  }

  q15_t txLevel = 0;
  switch (mode) {
    case STATE_DSTAR:
      txLevel = m_dstarTXLevel;
      break;
    case STATE_DMR:
      txLevel = m_dmrTXLevel;
      break;
    case STATE_YSF:
      txLevel = m_ysfTXLevel;
      break;
    case STATE_P25:
      txLevel = m_p25TXLevel;
      break;
    default:
      txLevel = m_cwIdTXLevel;
      break;
  }

  for (uint16_t i = 0U; i < length; i++) {
    q31_t res1 = samples[i] * txLevel;
    q15_t res2 = q15_t(__SSAT((res1 >> 15), 16));
    uint16_t res3 = uint16_t(res2 + DC_OFFSET);

    // Detect DAC overflow
    if (res3 > 4095U)
      m_dacOverflow++;

    if (control == NULL)
      m_txBuffer.put(res3, MARK_NONE);
    else
      m_txBuffer.put(res3, control[i]);
  }
}

uint16_t CIO::getSpace() const
{
  return m_txBuffer.getSpace();
}

void CIO::setDecode(bool dcd)
{
  if (dcd != m_dcd)
    setCOSInt(dcd ? true : false);

  m_dcd = dcd;
}

void CIO::setADCDetection(bool detect)
{
  m_detect = detect;
}

void CIO::setMode()
{
#if defined(ARDUINO_MODE_PINS)
  setDStarInt(m_modemState == STATE_DSTAR);
  setDMRInt(m_modemState   == STATE_DMR);
  setYSFInt(m_modemState   == STATE_YSF);
  setP25Int(m_modemState   == STATE_P25);
#endif
}

void CIO::setParameters(bool rxInvert, bool txInvert, bool pttInvert, uint8_t rxLevel, uint8_t cwIdTXLevel, uint8_t dstarTXLevel, uint8_t dmrTXLevel, uint8_t ysfTXLevel, uint8_t p25TXLevel)
{
  m_pttInvert = pttInvert;

  m_rxLevel      = q15_t(rxLevel * 128);
  m_cwIdTXLevel  = q15_t(cwIdTXLevel * 128);
  m_dstarTXLevel = q15_t(dstarTXLevel * 128);
  m_dmrTXLevel   = q15_t(dmrTXLevel * 128);
  m_ysfTXLevel   = q15_t(ysfTXLevel * 128);
  m_p25TXLevel   = q15_t(p25TXLevel * 128);
  
  if (rxInvert)
    m_rxLevel = -m_rxLevel;
  
  if (txInvert) {
    m_dstarTXLevel = -m_dstarTXLevel;
    m_dmrTXLevel   = -m_dmrTXLevel;
    m_ysfTXLevel   = -m_ysfTXLevel;
    m_p25TXLevel   = -m_p25TXLevel;
  }
}

void CIO::getOverflow(bool& adcOverflow, bool& dacOverflow)
{
  adcOverflow = m_adcOverflow > 0U;
  dacOverflow = m_dacOverflow > 0U;

  m_adcOverflow = 0U;
  m_dacOverflow = 0U;
}

bool CIO::hasTXOverflow()
{
  return m_txBuffer.hasOverflowed();
}

bool CIO::hasRXOverflow()
{
  return m_rxBuffer.hasOverflowed();
}

void CIO::resetWatchdog()
{
  m_watchdog = 0U;
}

bool CIO::hasLockout() const
{
  return m_lockout;
}

