/*
 *   Copyright (C) 2015,2016,2017,2018,2020,2021 by Jonathan Naylor G4KLX
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

#if defined(USE_DCBLOCKER)
// Generated using [b, a] = butter(1, 0.001) in MATLAB
static q31_t   DC_FILTER[] = {3367972, 0, 3367972, 0, 2140747704, 0}; // {b0, 0, b1, b2, -a1, -a2}
const uint32_t DC_FILTER_STAGES = 1U; // One Biquad stage
#endif

#if defined(MODE_DMR) || defined(MODE_YSF)
// Generated using rcosdesign(0.2, 8, 5, 'sqrt') in MATLAB
static q15_t RRC_0_2_FILTER[] = {401, 104, -340, -731, -847, -553, 112, 909, 1472, 1450, 683, -675, -2144, -3040, -2706, -770, 2667, 6995,
                                 11237, 14331, 15464, 14331, 11237, 6995, 2667, -770, -2706, -3040, -2144, -675, 683, 1450, 1472, 909, 112,
                                 -553, -847, -731, -340, 104, 401, 0};
const uint16_t RRC_0_2_FILTER_LEN = 42U;
#endif

#if defined(MODE_M17)
// Generated using rcosdesign(0.5, 8, 5, 'sqrt') in MATLAB
static q15_t RRC_0_5_FILTER[] = {-147, -88, 72, 220, 223, 46, -197, -285, -79, 334, 623, 390, -498, -1691, -2363, -1556, 1284, 5872, 11033,
				  15109, 16656, 15109, 11033, 5872, 1284, -1556, -2363, -1691, -498, 390, 623, 334, -79, -285, -197, 46, 223,
				  220, 72, -88, -147, 0};
const uint16_t RRC_0_5_FILTER_LEN = 42U;
#endif

#if defined(MODE_NXDN)
#if defined(USE_NXDN_BOXCAR)
// One symbol boxcar filter
static q15_t   BOXCAR10_FILTER[] = {6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000};
const uint16_t BOXCAR10_FILTER_LEN = 10U;
#else
// Generated using rcosdesign(0.2, 8, 10, 'sqrt') in MATLAB
static q15_t NXDN_0_2_FILTER[] = {284, 198, 73, -78, -240, -393, -517, -590, -599, -533, -391, -181, 79, 364, 643, 880, 1041, 1097, 1026, 819,
                                  483, 39, -477, -1016, -1516, -1915, -2150, -2164, -1914, -1375, -545, 557, 1886, 3376, 4946, 6502, 7946, 9184,
                                  10134, 10731, 10935, 10731, 10134, 9184, 7946, 6502, 4946, 3376, 1886, 557, -545, -1375, -1914, -2164, -2150,
                                  -1915, -1516, -1016, -477, 39, 483, 819, 1026, 1097, 1041, 880, 643, 364, 79, -181, -391, -533, -599, -590,
                                  -517, -393, -240, -78, 73, 198, 284, 0};
const uint16_t NXDN_0_2_FILTER_LEN = 82U;

static q15_t NXDN_ISINC_FILTER[] = {790, -1085, -1073, -553, 747, 2341, 3156, 2152, -893, -4915, -7834, -7536, -3102, 4441, 12354, 17394, 17394,
                                   12354, 4441, -3102, -7536, -7834, -4915, -893, 2152, 3156, 2341, 747, -553, -1073, -1085, 790};
const uint16_t NXDN_ISINC_FILTER_LEN = 32U;
#endif
#endif

#if defined(MODE_DSTAR)
// Generated using gaussfir(0.5, 4, 5) in MATLAB
static q15_t   GAUSSIAN_0_5_FILTER[] = {8, 104, 760, 3158, 7421, 9866, 7421, 3158, 760, 104, 8, 0};
const uint16_t GAUSSIAN_0_5_FILTER_LEN = 12U;
#endif

#if defined(MODE_P25)
// One symbol boxcar filter
static q15_t   BOXCAR5_FILTER[] = {12000, 12000, 12000, 12000, 12000, 0};
const uint16_t BOXCAR5_FILTER_LEN = 6U;
#endif

const uint16_t DC_OFFSET = 2048U;

CIO::CIO() :
m_started(false),
m_rxBuffer(RX_RINGBUFFER_SIZE),
m_txBuffer(TX_RINGBUFFER_SIZE),
m_rssiBuffer(RX_RINGBUFFER_SIZE),
#if defined(USE_DCBLOCKER)
m_dcFilter(),
m_dcState(),
#endif
#if defined(MODE_DSTAR)
m_gaussianFilter(),
m_gaussianState(),
#endif
#if defined(MODE_DMR)
m_rrc02Filter1(),
m_rrc02State1(),
#endif
#if defined(MODE_YSF)
m_rrc02Filter2(),
m_rrc02State2(),
#endif
#if defined(MODE_P25)
m_boxcar5Filter(),
m_boxcar5State(),
#endif
#if defined(MODE_NXDN)
#if defined(USE_NXDN_BOXCAR)
m_boxcar10Filter(),
m_boxcar10State(),
#else
m_nxdnFilter(),
m_nxdnISincFilter(),
m_nxdnState(),
m_nxdnISincState(),
#endif
#endif
#if defined(MODE_M17)
m_rrc05Filter(),
m_rrc05State(),
#endif
m_pttInvert(false),
m_rxLevel(128 * 128),
m_cwIdTXLevel(128 * 128),
m_dstarTXLevel(128 * 128),
m_dmrTXLevel(128 * 128),
m_ysfTXLevel(128 * 128),
m_p25TXLevel(128 * 128),
m_nxdnTXLevel(128 * 128),
m_m17TXLevel(128 * 128),
m_pocsagTXLevel(128 * 128),
m_fmTXLevel(128 * 128),
m_ax25TXLevel(128 * 128),
m_rxDCOffset(DC_OFFSET),
m_txDCOffset(DC_OFFSET),
m_useCOSAsLockout(false),
m_ledCount(0U),
m_ledValue(true),
m_detect(false),
m_adcOverflow(0U),
m_dacOverflow(0U),
m_watchdog(0U),
m_lockout(false)
{
#if defined(USE_DCBLOCKER)
  ::memset(m_dcState, 0x00U, 4U * sizeof(q31_t));
  m_dcFilter.numStages = DC_FILTER_STAGES;
  m_dcFilter.pState    = m_dcState;
  m_dcFilter.pCoeffs   = DC_FILTER;
  m_dcFilter.postShift = 0;
#endif

#if defined(MODE_DSTAR)
  ::memset(m_gaussianState, 0x00U, 40U * sizeof(q15_t));
  m_gaussianFilter.numTaps = GAUSSIAN_0_5_FILTER_LEN;
  m_gaussianFilter.pState  = m_gaussianState;
  m_gaussianFilter.pCoeffs = GAUSSIAN_0_5_FILTER;
#endif

#if defined(MODE_DMR)
  ::memset(m_rrc02State1, 0x00U, 70U * sizeof(q15_t));
  m_rrc02Filter1.numTaps = RRC_0_2_FILTER_LEN;
  m_rrc02Filter1.pState  = m_rrc02State1;
  m_rrc02Filter1.pCoeffs = RRC_0_2_FILTER;
#endif

#if defined(MODE_YSF)
  ::memset(m_rrc02State2, 0x00U, 70U * sizeof(q15_t));
  m_rrc02Filter2.numTaps = RRC_0_2_FILTER_LEN;
  m_rrc02Filter2.pState  = m_rrc02State2;
  m_rrc02Filter2.pCoeffs = RRC_0_2_FILTER;
#endif

#if defined(MODE_P25)
  ::memset(m_boxcar5State, 0x00U, 30U * sizeof(q15_t));
  m_boxcar5Filter.numTaps = BOXCAR5_FILTER_LEN;
  m_boxcar5Filter.pState  = m_boxcar5State;
  m_boxcar5Filter.pCoeffs = BOXCAR5_FILTER;
#endif

#if defined(MODE_NXDN)
#if defined(USE_NXDN_BOXCAR)
  ::memset(m_boxcar10State, 0x00U, 40U * sizeof(q15_t));
  m_boxcar10Filter.numTaps = BOXCAR10_FILTER_LEN;
  m_boxcar10Filter.pState  = m_boxcar10State;
  m_boxcar10Filter.pCoeffs = BOXCAR10_FILTER;
#else
  ::memset(m_nxdnState,      0x00U, 110U * sizeof(q15_t));
  ::memset(m_nxdnISincState, 0x00U, 60U * sizeof(q15_t));

  m_nxdnFilter.numTaps = NXDN_0_2_FILTER_LEN;
  m_nxdnFilter.pState  = m_nxdnState;
  m_nxdnFilter.pCoeffs = NXDN_0_2_FILTER;
  
  m_nxdnISincFilter.numTaps = NXDN_ISINC_FILTER_LEN;
  m_nxdnISincFilter.pState  = m_nxdnISincState;
  m_nxdnISincFilter.pCoeffs = NXDN_ISINC_FILTER;
#endif
#endif

#if defined(MODE_M17)
  ::memset(m_rrc05State, 0x00U, 70U * sizeof(q15_t));
  m_rrc05Filter.numTaps = RRC_0_5_FILTER_LEN;
  m_rrc05Filter.pState  = m_rrc05State;
  m_rrc05Filter.pCoeffs = RRC_0_5_FILTER;
#endif

  initInt();
  
  selfTest();
}

void CIO::selfTest()
{
  bool ledValue = false;

  for (uint8_t i = 0U; i < 6U; i++) {
    ledValue = !ledValue;

    // We exclude PTT to avoid trigger the transmitter
    setLEDInt(ledValue);
    setCOSInt(ledValue);
#if defined(MODE_LEDS)
    setDStarInt(ledValue);
    setDMRInt(ledValue);
    setYSFInt(ledValue);
    setP25Int(ledValue);
#if !defined(USE_ALTERNATE_NXDN_LEDS)
    setNXDNInt(ledValue);
#endif
#if !defined(USE_ALTERNATE_M17_LEDS)
    setM17Int(ledValue);
#endif
#if !defined(USE_ALTERNATE_POCSAG_LEDS)
    setPOCSAGInt(ledValue);
#endif
#if !defined(USE_ALTERNATE_FM_LEDS)
    setFMInt(ledValue);
#endif
#endif
    delayInt(250);
  }

#if defined(MODE_LEDS)
  setDStarInt(false);
  setDMRInt(false);
  setYSFInt(false);
  setP25Int(false);
#if !defined(USE_ALTERNATE_NXDN_LEDS)
  setNXDNInt(false);
#endif
#if !defined(USE_ALTERNATE_M17_LEDS)
  setM17Int(false);
#endif
#if !defined(USE_ALTERNATE_POCSAG_LEDS)
  setPOCSAGInt(false);
#endif
#if !defined(USE_ALTERNATE_FM_LEDS)
  setFMInt(false);
#endif
  setDStarInt(true);

  delayInt(250);
  setDMRInt(true);

  delayInt(250);
  setYSFInt(true);

  delayInt(250);
  setP25Int(true);

#if !defined(USE_ALTERNATE_NXDN_LEDS)
  delayInt(250);
  setNXDNInt(true);
#endif

#if !defined(USE_ALTERNATE_M17_LEDS)
  delayInt(250);
  setM17Int(true);
#endif

#if !defined(USE_ALTERNATE_POCSAG_LEDS)
  delayInt(250);
  setPOCSAGInt(true);
#endif

#if !defined(USE_ALTERNATE_FM_LEDS)
  delayInt(250);
  setFMInt(true);

  delayInt(250);
  setFMInt(false);
#endif

#if !defined(USE_ALTERNATE_POCSAG_LEDS)
  delayInt(250);
  setPOCSAGInt(false);
#endif

#if !defined(USE_ALTERNATE_M17_LEDS)
  delayInt(250);
  setM17Int(false);
#endif

#if !defined(USE_ALTERNATE_NXDN_LEDS)
  delayInt(250);
  setNXDNInt(false);
#endif

  delayInt(250);
  setP25Int(false);

  delayInt(250);
  setYSFInt(false);

  delayInt(250);
  setDMRInt(false);

  delayInt(250);
  setDStarInt(false);
#endif
}

void CIO::start()
{
  if (m_started)
    return;

  startInt();

  m_started = true;

  setMode(STATE_IDLE);
}

void CIO::process()
{
  m_ledCount++;
  if (m_started) {
    // Two seconds timeout
    if (m_watchdog >= 48000U) {
      if (m_modemState == STATE_DSTAR || m_modemState == STATE_DMR || m_modemState == STATE_YSF || m_modemState == STATE_P25 || m_modemState == STATE_NXDN || m_modemState == STATE_M17 || m_modemState == STATE_POCSAG) {
#if defined(MODE_DMR)
        if (m_modemState == STATE_DMR && m_tx)
          dmrTX.setStart(false);
#endif
        setMode(STATE_IDLE);
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

  if (m_useCOSAsLockout)
    m_lockout = getCOSInt();

  // Switch off the transmitter if needed
  if (m_txBuffer.getData() == 0U && m_tx) {
    m_tx = false;
    setPTTInt(m_pttInvert ? true : false);
    DEBUG1("TX OFF");
  }

  if (m_rxBuffer.getData() >= RX_BLOCK_SIZE) {
    q15_t    samples[RX_BLOCK_SIZE];
    uint8_t  control[RX_BLOCK_SIZE];
    uint16_t rssi[RX_BLOCK_SIZE];

    for (uint16_t i = 0U; i < RX_BLOCK_SIZE; i++) {
      TSample sample;
      m_rxBuffer.get(sample);
      control[i] = sample.control;
      m_rssiBuffer.get(rssi[i]);

      // Detect ADC overflow
      if (m_detect && (sample.sample == 0U || sample.sample == 4095U))
        m_adcOverflow++;

      q15_t res1 = q15_t(sample.sample) - m_rxDCOffset;
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
#if defined(MODE_DSTAR)
      if (m_dstarEnable) {
        q15_t GMSKVals[RX_BLOCK_SIZE];
#if defined(USE_DCBLOCKER)
        ::arm_fir_fast_q15(&m_gaussianFilter, dcSamples, GMSKVals, RX_BLOCK_SIZE);
#else
        ::arm_fir_fast_q15(&m_gaussianFilter, samples, GMSKVals, RX_BLOCK_SIZE);
#endif
        dstarRX.samples(GMSKVals, rssi, RX_BLOCK_SIZE);
      }
#endif

#if defined(MODE_P25)
      if (m_p25Enable) {
        q15_t P25Vals[RX_BLOCK_SIZE];
#if defined(USE_DCBLOCKER)
        ::arm_fir_fast_q15(&m_boxcar5Filter, dcSamples, P25Vals, RX_BLOCK_SIZE);
#else
        ::arm_fir_fast_q15(&m_boxcar5Filter, samples, P25Vals, RX_BLOCK_SIZE);
#endif
        p25RX.samples(P25Vals, rssi, RX_BLOCK_SIZE);
      }
#endif

#if defined(MODE_NXDN)
      if (m_nxdnEnable) {
        q15_t NXDNVals[RX_BLOCK_SIZE];
#if defined(USE_NXDN_BOXCAR)
#if defined(USE_DCBLOCKER)
        ::arm_fir_fast_q15(&m_boxcar10Filter, dcSamples, NXDNVals, RX_BLOCK_SIZE);
#else
        ::arm_fir_fast_q15(&m_boxcar10Filter, samples, NXDNVals, RX_BLOCK_SIZE);
#endif
#else
        q15_t NXDNValsTmp[RX_BLOCK_SIZE];
#if defined(USE_DCBLOCKER)
        ::arm_fir_fast_q15(&m_nxdnFilter, dcSamples, NXDNValsTmp, RX_BLOCK_SIZE);
#else
        ::arm_fir_fast_q15(&m_nxdnFilter, samples, NXDNValsTmp, RX_BLOCK_SIZE);
#endif
        ::arm_fir_fast_q15(&m_nxdnISincFilter, NXDNValsTmp, NXDNVals, RX_BLOCK_SIZE);
#endif
        nxdnRX.samples(NXDNVals, rssi, RX_BLOCK_SIZE);
      }
#endif

#if defined(MODE_DMR)
      if (m_dmrEnable) {
        q15_t DMRVals[RX_BLOCK_SIZE];
        ::arm_fir_fast_q15(&m_rrc02Filter1, samples, DMRVals, RX_BLOCK_SIZE);

        if (m_duplex)
          dmrIdleRX.samples(DMRVals, RX_BLOCK_SIZE);
        else
          dmrDMORX.samples(DMRVals, rssi, RX_BLOCK_SIZE);
      }
#endif

#if defined(MODE_YSF)
      if (m_ysfEnable) {
        q15_t YSFVals[RX_BLOCK_SIZE];
#if defined(USE_DCBLOCKER)
        ::arm_fir_fast_q15(&m_rrc02Filter2, dcSamples, YSFVals, RX_BLOCK_SIZE);
#else
        ::arm_fir_fast_q15(&m_rrc02Filter2, samples, YSFVals, RX_BLOCK_SIZE);
#endif
        ysfRX.samples(YSFVals, rssi, RX_BLOCK_SIZE);
      }
#endif

#if defined(MODE_M17)
      if (m_m17Enable) {
        q15_t RRCVals[RX_BLOCK_SIZE];
#if defined(USE_DCBLOCKER)
        ::arm_fir_fast_q15(&m_rrc05Filter, dcSamples, RRCVals, RX_BLOCK_SIZE);
#else
        ::arm_fir_fast_q15(&m_rrc05Filter, samples, RRCVals, RX_BLOCK_SIZE);
#endif
        m17RX.samples(RRCVals, rssi, RX_BLOCK_SIZE);
      }
#endif

#if defined(MODE_FM)
      if (m_fmEnable) {
        bool cos = getCOSInt();
#if defined(USE_DCBLOCKER)
        fm.samples(cos, dcSamples, RX_BLOCK_SIZE);
#else
        fm.samples(cos, samples, RX_BLOCK_SIZE);
#endif
      }
#endif

#if defined(MODE_FM) && defined(MODE_AX25)
      if (m_ax25Enable) {
#if defined(USE_DCBLOCKER)
        ax25RX.samples(dcSamples, RX_BLOCK_SIZE);
#else
        ax25RX.samples(samples, RX_BLOCK_SIZE);
#endif
      }
#endif
    }

#if defined(MODE_DSTAR)
    else if (m_modemState == STATE_DSTAR) {
      if (m_dstarEnable) {
        q15_t GMSKVals[RX_BLOCK_SIZE];
#if defined(USE_DCBLOCKER)
        ::arm_fir_fast_q15(&m_gaussianFilter, dcSamples, GMSKVals, RX_BLOCK_SIZE);
#else
        ::arm_fir_fast_q15(&m_gaussianFilter, samples, GMSKVals, RX_BLOCK_SIZE);
#endif
        dstarRX.samples(GMSKVals, rssi, RX_BLOCK_SIZE);
      }
    }
#endif

#if defined(MODE_DMR)
    else if (m_modemState == STATE_DMR) {
      if (m_dmrEnable) {
        q15_t DMRVals[RX_BLOCK_SIZE];
        ::arm_fir_fast_q15(&m_rrc02Filter1, samples, DMRVals, RX_BLOCK_SIZE);

        if (m_duplex) {
          // If the transmitter isn't on, use the DMR idle RX to detect the wakeup CSBKs
          if (m_tx)
            dmrRX.samples(DMRVals, rssi, control, RX_BLOCK_SIZE);
          else
            dmrIdleRX.samples(DMRVals, RX_BLOCK_SIZE);
        } else {
          dmrDMORX.samples(DMRVals, rssi, RX_BLOCK_SIZE);
        }
      }
    }
#endif

#if defined(MODE_YSF)
    else if (m_modemState == STATE_YSF) {
      if (m_ysfEnable) {
        q15_t YSFVals[RX_BLOCK_SIZE];
#if defined(USE_DCBLOCKER)
        ::arm_fir_fast_q15(&m_rrc02Filter2, dcSamples, YSFVals, RX_BLOCK_SIZE);
#else
        ::arm_fir_fast_q15(&m_rrc02Filter2, samples, YSFVals, RX_BLOCK_SIZE);
#endif
        ysfRX.samples(YSFVals, rssi, RX_BLOCK_SIZE);
      }
    }
#endif

#if defined(MODE_P25)
    else if (m_modemState == STATE_P25) {
      if (m_p25Enable) {
        q15_t P25Vals[RX_BLOCK_SIZE];
#if defined(USE_DCBLOCKER)
        ::arm_fir_fast_q15(&m_boxcar5Filter, dcSamples, P25Vals, RX_BLOCK_SIZE);
#else
        ::arm_fir_fast_q15(&m_boxcar5Filter, samples, P25Vals, RX_BLOCK_SIZE);
#endif
        p25RX.samples(P25Vals, rssi, RX_BLOCK_SIZE);
      }
    }
#endif

#if defined(MODE_NXDN)
    else if (m_modemState == STATE_NXDN) {
      if (m_nxdnEnable) {
        q15_t NXDNVals[RX_BLOCK_SIZE];
#if defined(USE_NXDN_BOXCAR)
#if defined(USE_DCBLOCKER)
        ::arm_fir_fast_q15(&m_boxcar10Filter, dcSamples, NXDNVals, RX_BLOCK_SIZE);
#else
        ::arm_fir_fast_q15(&m_boxcar10Filter, samples, NXDNVals, RX_BLOCK_SIZE);
#endif
#else
        q15_t NXDNValsTmp[RX_BLOCK_SIZE];
#if defined(USE_DCBLOCKER)
        ::arm_fir_fast_q15(&m_nxdnFilter, dcSamples, NXDNValsTmp, RX_BLOCK_SIZE);
#else
        ::arm_fir_fast_q15(&m_nxdnFilter, samples, NXDNValsTmp, RX_BLOCK_SIZE);
#endif
        ::arm_fir_fast_q15(&m_nxdnISincFilter, NXDNValsTmp, NXDNVals, RX_BLOCK_SIZE);
#endif
        nxdnRX.samples(NXDNVals, rssi, RX_BLOCK_SIZE);
      }
    }
#endif

#if defined(MODE_M17)
    else if (m_modemState == STATE_M17) {
      if (m_m17Enable) {
        q15_t M17Vals[RX_BLOCK_SIZE];
#if defined(USE_DCBLOCKER)
        ::arm_fir_fast_q15(&m_rrc05Filter, dcSamples, M17Vals, RX_BLOCK_SIZE);
#else
        ::arm_fir_fast_q15(&m_rrc05Filter, samples, M17Vals, RX_BLOCK_SIZE);
#endif
        m17RX.samples(M17Vals, rssi, RX_BLOCK_SIZE);
      }
    }
#endif

#if defined(MODE_FM)
    else if (m_modemState == STATE_FM) {
      bool cos = getCOSInt();
#if defined(USE_DCBLOCKER)
      fm.samples(cos, dcSamples, RX_BLOCK_SIZE);

#if defined(MODE_AX25)
      if (m_ax25Enable)
        ax25RX.samples(dcSamples, RX_BLOCK_SIZE);
#endif
#else
      fm.samples(cos, samples, RX_BLOCK_SIZE);

#if defined(MODE_AX25)
      if (m_ax25Enable)
        ax25RX.samples(samples, RX_BLOCK_SIZE);
#endif
#endif
    }
#endif

#if defined(MODE_DSTAR)
    else if (m_modemState == STATE_DSTARCAL) {
      q15_t GMSKVals[RX_BLOCK_SIZE];
      ::arm_fir_fast_q15(&m_gaussianFilter, samples, GMSKVals, RX_BLOCK_SIZE);

      calDStarRX.samples(GMSKVals, RX_BLOCK_SIZE);
    }
#endif

    else if (m_modemState == STATE_RSSICAL) {
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
    DEBUG1("TX ON");
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
    case STATE_M17:
      txLevel = m_m17TXLevel;
      break;
    case STATE_POCSAG:
      txLevel = m_pocsagTXLevel;
      break;
    case STATE_FM:
      txLevel = m_fmTXLevel;
      break;
    case STATE_AX25:
      txLevel = m_ax25TXLevel;
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
      m_txBuffer.put({res3, MARK_NONE});
    else
      m_txBuffer.put({res3, control[i]});
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

void CIO::setMode(MMDVM_STATE state)
{
  if (state == m_modemState)
    return;

#if defined(MODE_LEDS)
  switch (m_modemState) {
    case STATE_DSTAR:  setDStarInt(false);  break;
    case STATE_DMR:    setDMRInt(false);    break;
    case STATE_YSF:    setYSFInt(false);    break;
    case STATE_P25:    setP25Int(false);    break;
    case STATE_NXDN:   setNXDNInt(false);   break;
    case STATE_M17:    setM17Int(false);    break;
    case STATE_POCSAG: setPOCSAGInt(false); break;
    case STATE_FM:     setFMInt(false);     break;
    default: break;
  }

  switch (state) {
    case STATE_DSTAR:  setDStarInt(true);  break;
    case STATE_DMR:    setDMRInt(true);    break;
    case STATE_YSF:    setYSFInt(true);    break;
    case STATE_P25:    setP25Int(true);    break;
    case STATE_NXDN:   setNXDNInt(true);   break;
    case STATE_M17:    setM17Int(true);    break;
    case STATE_POCSAG: setPOCSAGInt(true); break;
    case STATE_FM:     setFMInt(true);     break;
    default: break;
  }
#endif

  m_modemState = state;
}

void CIO::setParameters(bool rxInvert, bool txInvert, bool pttInvert, uint8_t rxLevel, uint8_t cwIdTXLevel, uint8_t dstarTXLevel, uint8_t dmrTXLevel, uint8_t ysfTXLevel, uint8_t p25TXLevel, uint8_t nxdnTXLevel, uint8_t m17TXLevel, uint8_t pocsagTXLevel, uint8_t fmTXLevel, uint8_t ax25TXLevel, int16_t txDCOffset, int16_t rxDCOffset, bool useCOSAsLockout)
{
  m_pttInvert = pttInvert;

  m_rxLevel       = q15_t(rxLevel * 128);
  m_cwIdTXLevel   = q15_t(cwIdTXLevel * 128);
  m_dstarTXLevel  = q15_t(dstarTXLevel * 128);
  m_dmrTXLevel    = q15_t(dmrTXLevel * 128);
  m_ysfTXLevel    = q15_t(ysfTXLevel * 128);
  m_p25TXLevel    = q15_t(p25TXLevel * 128);
  m_nxdnTXLevel   = q15_t(nxdnTXLevel * 128);
  m_m17TXLevel    = q15_t(m17TXLevel * 128);
  m_pocsagTXLevel = q15_t(pocsagTXLevel * 128);
  m_fmTXLevel     = q15_t(fmTXLevel * 128);
  m_ax25TXLevel   = q15_t(ax25TXLevel * 128);

  m_rxDCOffset   = DC_OFFSET + rxDCOffset;
  m_txDCOffset   = DC_OFFSET + txDCOffset;

  m_useCOSAsLockout = useCOSAsLockout;
  
  if (rxInvert)
    m_rxLevel = -m_rxLevel;
  
  if (txInvert) {
    m_dstarTXLevel  = -m_dstarTXLevel;
    m_dmrTXLevel    = -m_dmrTXLevel;
    m_ysfTXLevel    = -m_ysfTXLevel;
    m_p25TXLevel    = -m_p25TXLevel;
    m_nxdnTXLevel   = -m_nxdnTXLevel;
    m_m17TXLevel    = -m_m17TXLevel;
    m_pocsagTXLevel = -m_pocsagTXLevel;
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
