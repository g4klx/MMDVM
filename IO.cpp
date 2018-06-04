/*
 *   Copyright (C) 2015,2016,2017,2018 by Jonathan Naylor G4KLX
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

// One symbol boxcar filters
static q15_t   BOXCAR5_FILTER[] = {12000, 12000, 12000, 12000, 12000, 0};
const uint16_t BOXCAR5_FILTER_LEN = 6U;

static q15_t   BOXCAR10_FILTER[] = {6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000};
const uint16_t BOXCAR10_FILTER_LEN = 10U;

const uint16_t DC_OFFSET = 2048U;

CIO::CIO() :
m_started(false),
m_rxBuffer(RX_RINGBUFFER_SIZE),
m_txBuffer(TX_RINGBUFFER_SIZE),
m_rssiBuffer(RX_RINGBUFFER_SIZE),
m_dcFilter(),
m_dcState(),
m_boxcar5Filter(),
m_boxcar10Filter(),
m_boxcar5State(),
m_boxcar10State(),
m_pttInvert(false),
m_rxLevel(128 * 128),
m_cwIdTXLevel(128 * 128),
m_dstarTXLevel(128 * 128),
m_dmrTXLevel(128 * 128),
m_ysfTXLevel(128 * 128),
m_p25TXLevel(128 * 128),
m_nxdnTXLevel(128 * 128),
m_rxDCOffset(DC_OFFSET),
m_txDCOffset(DC_OFFSET),
m_ledCount(0U),
m_ledValue(true),
m_detect(false),
m_adcOverflow(0U),
m_dacOverflow(0U),
m_watchdog(0U),
m_lockout(false)
{
  ::memset(m_boxcar5State,  0x00U, 30U * sizeof(q15_t));
  ::memset(m_boxcar10State, 0x00U, 40U * sizeof(q15_t));
  ::memset(m_dcState,       0x00U,  4U * sizeof(q31_t));

  m_dcFilter.numStages = DC_FILTER_STAGES;
  m_dcFilter.pState    = m_dcState;
  m_dcFilter.pCoeffs   = DC_FILTER;
  m_dcFilter.postShift = 0;

  m_boxcar5Filter.numTaps = BOXCAR5_FILTER_LEN;
  m_boxcar5Filter.pState  = m_boxcar5State;
  m_boxcar5Filter.pCoeffs = BOXCAR5_FILTER;

  m_boxcar10Filter.numTaps = BOXCAR10_FILTER_LEN;
  m_boxcar10Filter.pState  = m_boxcar10State;
  m_boxcar10Filter.pCoeffs = BOXCAR10_FILTER;

  initInt();
  
  selfTest();
}

void CIO::selfTest()
{
  bool ledValue = false;

  for (uint8_t i = 0; i < 6; i++) {
    ledValue = !ledValue;

    // We exclude PTT to avoid trigger the transmitter
    setLEDInt(ledValue);
    setCOSInt(ledValue);
#if defined(ARDUINO_MODE_PINS)
    setDStarInt(ledValue);
    setDMRInt(ledValue);
    setYSFInt(ledValue);
    setP25Int(ledValue);
    setNXDNInt(ledValue);
#endif
    delayInt(250);
  }

#if defined(ARDUINO_MODE_PINS)
  setDStarInt(true);
  setDMRInt(false);
  setYSFInt(false);
  setP25Int(false);
  setNXDNInt(false);

  delayInt(250);

  setDStarInt(true);
  setDMRInt(true);
  setYSFInt(false);
  setP25Int(false);
  setNXDNInt(false);

  delayInt(250);

  setDStarInt(true);
  setDMRInt(true);
  setYSFInt(true);
  setP25Int(false);
  setNXDNInt(false);
  
  delayInt(250);

  setDStarInt(true);
  setDMRInt(true);
  setYSFInt(true);
  setP25Int(true);
  setNXDNInt(false);
  
  delayInt(250);

  setDStarInt(true);
  setDMRInt(true);
  setYSFInt(true);
  setP25Int(true);
  setNXDNInt(true);
  
  delayInt(250);

  setDStarInt(true);
  setDMRInt(true);
  setYSFInt(true);
  setP25Int(true);
  setNXDNInt(false);

  delayInt(250);
  
  setDStarInt(true);
  setDMRInt(true);
  setYSFInt(true);
  setP25Int(false);
  setNXDNInt(false);

  delayInt(250);

  setDStarInt(true);
  setDMRInt(true);
  setYSFInt(false);
  setP25Int(false);
  setNXDNInt(false);

  delayInt(250);
  
  setDStarInt(true);
  setDMRInt(false);
  setYSFInt(false);
  setP25Int(false);
  setNXDNInt(false);

  delayInt(250);

  setDStarInt(false);
  setDMRInt(false);
  setYSFInt(false);
  setP25Int(false);
  setNXDNInt(false);
#endif
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
      if (m_modemState == STATE_DSTAR || m_modemState == STATE_DMR || m_modemState == STATE_YSF || m_modemState == STATE_P25 || m_modemState == STATE_NXDN) {
        if (m_modemState == STATE_DMR && m_tx)
          dmrTX.setStart(false);
        m_modemState = STATE_IDLE;
        setMode();
      }

      m_watchdog = 0U;
    }

#if defined(CONSTANT_SRV_LED)
    setLEDInt(true);
#else
    if (m_ledCount >= 24000U) {
      m_ledCount = 0U;
      m_ledValue = !m_ledValue;
      setLEDInt(m_ledValue);
    }
#endif
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

      q15_t res1 = q15_t(sample) - m_rxDCOffset;
      q31_t res2 = res1 * m_rxLevel;
      samples[i] = q15_t(__SSAT((res2 >> 15), 16));
    }

    if (m_lockout)
      return;

#if defined(USE_DCBLOCKER)
    q31_t q31Samples[RX_BLOCK_SIZE];
    ::arm_q15_to_q31(samples, q31Samples, RX_BLOCK_SIZE);

    q31_t dcValues[RX_BLOCK_SIZE];
    ::arm_biquad_cascade_df1_q31(&m_dcFilter, q31Samples, dcValues, RX_BLOCK_SIZE);

    q31_t dcLevel = 0;
    for (uint8_t i = 0U; i < RX_BLOCK_SIZE; i++)
      dcLevel += dcValues[i];
    dcLevel /= RX_BLOCK_SIZE;

    q15_t offset = q15_t(__SSAT((dcLevel >> 16), 16));;

    q15_t dcSamples[RX_BLOCK_SIZE];
    for (uint8_t i = 0U; i < RX_BLOCK_SIZE; i++)
      dcSamples[i] = samples[i] - offset;
#endif

    if (m_modemState == STATE_IDLE) {
      if (m_dstarEnable || m_dmrEnable || m_ysfEnable || m_p25Enable) {
        q15_t vals[RX_BLOCK_SIZE];
        ::arm_fir_fast_q15(&m_boxcar5Filter, samples, vals, RX_BLOCK_SIZE);

        if (m_dstarEnable)
          dstarRX.samples(vals, rssi, RX_BLOCK_SIZE);

        if (m_p25Enable)
          p25RX.samples(vals, rssi, RX_BLOCK_SIZE);

        if (m_ysfEnable)
          ysfRX.samples(vals, rssi, RX_BLOCK_SIZE);

        if (m_dmrEnable) {
          if (m_duplex)
            dmrIdleRX.samples(vals, RX_BLOCK_SIZE);
          else
            dmrDMORX.samples(vals, rssi, RX_BLOCK_SIZE);
        }
      }

      if (m_nxdnEnable) {
        q15_t vals[RX_BLOCK_SIZE];
#if defined(USE_DCBLOCKER)
        ::arm_fir_fast_q15(&m_boxcar10Filter, dcSamples, vals, RX_BLOCK_SIZE);
#else
        ::arm_fir_fast_q15(&m_boxcar10Filter, samples, vals, RX_BLOCK_SIZE);
#endif

        nxdnRX.samples(vals, rssi, RX_BLOCK_SIZE);
      }
    } else if (m_modemState == STATE_DSTAR) {
      if (m_dstarEnable) {
        q15_t vals[RX_BLOCK_SIZE];
#if defined(USE_DCBLOCKER)
        ::arm_fir_fast_q15(&m_boxcar5Filter, dcSamples, vals, RX_BLOCK_SIZE);
#else
        ::arm_fir_fast_q15(&m_boxcar5Filter, samples, vals, RX_BLOCK_SIZE);
#endif

        dstarRX.samples(vals, rssi, RX_BLOCK_SIZE);
      }
    } else if (m_modemState == STATE_DMR) {
      if (m_dmrEnable) {
        q15_t vals[RX_BLOCK_SIZE];
        ::arm_fir_fast_q15(&m_boxcar5Filter, samples, vals, RX_BLOCK_SIZE);

        if (m_duplex) {
          // If the transmitter isn't on, use the DMR idle RX to detect the wakeup CSBKs
          if (m_tx)
            dmrRX.samples(vals, rssi, control, RX_BLOCK_SIZE);
          else
            dmrIdleRX.samples(vals, RX_BLOCK_SIZE);
        } else {
          dmrDMORX.samples(vals, rssi, RX_BLOCK_SIZE);
        }
      }
    } else if (m_modemState == STATE_YSF) {
      if (m_ysfEnable) {
        q15_t vals[RX_BLOCK_SIZE];
#if defined(USE_DCBLOCKER)
        ::arm_fir_fast_q15(&m_boxcar5Filter, dcSamples, vals, RX_BLOCK_SIZE);
#else
        ::arm_fir_fast_q15(&m_boxcar5Filter, samples, vals, RX_BLOCK_SIZE);
#endif
        ysfRX.samples(vals, rssi, RX_BLOCK_SIZE);
      }
    } else if (m_modemState == STATE_P25) {
      if (m_p25Enable) {
        q15_t vals[RX_BLOCK_SIZE];
#if defined(USE_DCBLOCKER)
        ::arm_fir_fast_q15(&m_boxcar5Filter, dcSamples, vals, RX_BLOCK_SIZE);
#else
        ::arm_fir_fast_q15(&m_boxcar5Filter, samples, vals, RX_BLOCK_SIZE);
#endif
        p25RX.samples(vals, rssi, RX_BLOCK_SIZE);
      }
	} else if (m_modemState == STATE_NXDN) {
      if (m_nxdnEnable) {
        q15_t vals[RX_BLOCK_SIZE];
#if defined(USE_DCBLOCKER)
        ::arm_fir_fast_q15(&m_boxcar10Filter, dcSamples, vals, RX_BLOCK_SIZE);
#else
        ::arm_fir_fast_q15(&m_boxcar10Filter, samples, vals, RX_BLOCK_SIZE);
#endif
        nxdnRX.samples(vals, rssi, RX_BLOCK_SIZE);
      }
    } else if (m_modemState == STATE_DSTARCAL) {
      q15_t vals[RX_BLOCK_SIZE];
#if defined(USE_DCBLOCKER)
      ::arm_fir_fast_q15(&m_boxcar5Filter, dcSamples, vals, RX_BLOCK_SIZE);
#else
      ::arm_fir_fast_q15(&m_boxcar5Filter, samples, vals, RX_BLOCK_SIZE);
#endif
      calDStarRX.samples(vals, RX_BLOCK_SIZE);
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
    case STATE_NXDN:
      txLevel = m_nxdnTXLevel;
      break;
    default:
      txLevel = m_cwIdTXLevel;
      break;
  }

  for (uint16_t i = 0U; i < length; i++) {
    q31_t res1 = samples[i] * txLevel;
    q15_t res2 = q15_t(__SSAT((res1 >> 15), 16));
    uint16_t res3 = uint16_t(res2 + m_txDCOffset);

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
  setNXDNInt(m_modemState  == STATE_NXDN);
#endif
}

void CIO::setParameters(bool rxInvert, bool txInvert, bool pttInvert, uint8_t rxLevel, uint8_t cwIdTXLevel, uint8_t dstarTXLevel, uint8_t dmrTXLevel, uint8_t ysfTXLevel, uint8_t p25TXLevel, uint8_t nxdnTXLevel, int16_t txDCOffset, int16_t rxDCOffset)
{
  m_pttInvert = pttInvert;

  m_rxLevel      = q15_t(rxLevel * 128);
  m_cwIdTXLevel  = q15_t(cwIdTXLevel * 128);
  m_dstarTXLevel = q15_t(dstarTXLevel * 128);
  m_dmrTXLevel   = q15_t(dmrTXLevel * 128);
  m_ysfTXLevel   = q15_t(ysfTXLevel * 128);
  m_p25TXLevel   = q15_t(p25TXLevel * 128);
  m_nxdnTXLevel  = q15_t(nxdnTXLevel * 128);

  m_rxDCOffset   = DC_OFFSET + rxDCOffset;
  m_txDCOffset   = DC_OFFSET + txDCOffset;
  
  if (rxInvert)
    m_rxLevel = -m_rxLevel;
  
  if (txInvert) {
    m_dstarTXLevel = -m_dstarTXLevel;
    m_dmrTXLevel   = -m_dmrTXLevel;
    m_ysfTXLevel   = -m_ysfTXLevel;
    m_p25TXLevel   = -m_p25TXLevel;
    m_nxdnTXLevel  = -m_nxdnTXLevel;
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

uint32_t CIO::getWatchdog()
{
  return m_watchdog;
}

bool CIO::hasLockout() const
{
  return m_lockout;
}

