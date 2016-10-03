/*
 *   Copyright (C) 2015,2016 by Jonathan Naylor G4KLX
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

// #define  WANT_DEBUG

#include "Config.h"
#include "Globals.h"
#include "IO.h"

#if defined(WIDE_C4FSK_FILTERS_RX)
// Generated using rcosdesign(0.2, 4, 5, 'sqrt') in MATLAB
static q15_t C4FSK_FILTER[] = {688, -680, -2158, -3060, -2724, -775, 2684, 7041, 11310, 14425, 15565, 14425,
                                   11310, 7041, 2684, -775, -2724, -3060, -2158, -680, 688, 0};
const uint16_t C4FSK_FILTER_LEN = 22U;
#else
// Generated using rcosdesign(0.2, 8, 5, 'sqrt') in MATLAB
static q15_t C4FSK_FILTER[] = {401, 104, -340, -731, -847, -553, 112, 909, 1472, 1450, 683, -675, -2144, -3040, -2706, -770, 2667, 6995,
                                   11237, 14331, 15464, 14331, 11237, 6995, 2667, -770, -2706, -3040, -2144, -675, 683, 1450, 1472, 909, 112,
                                   -553, -847, -731, -340, 104, 401, 0};
const uint16_t C4FSK_FILTER_LEN = 42U;
#endif

// Generated using gaussfir(0.5, 4, 5) in MATLAB
static q15_t   GMSK_FILTER[] = {8, 104, 760, 3158, 7421, 9866, 7421, 3158, 760, 104, 8, 0};
const uint16_t GMSK_FILTER_LEN = 12U;

const uint16_t DC_OFFSET = 2048U;

#if defined(__SAM3X8E__)
// An Arduino Due
#if defined(ARDUINO_DUE_PAPA)
#define PIN_COS                7
#define PIN_PTT                8
#define PIN_COSLED             11
#define ADC_CHER_Chan          (1<<7)                 // ADC on Due pin A0  - Due AD7 - (1 << 7)
#define ADC_ISR_EOC_Chan       ADC_ISR_EOC7
#define ADC_CDR_Chan           7
#define DACC_MR_USER_SEL_Chan  DACC_MR_USER_SEL_CHANNEL0 // DAC on Due DAC0
#define DACC_CHER_Chan         DACC_CHER_CH0
#elif defined(ARDUINO_DUE_ZUM_V10)
#define PIN_COS                52
#define PIN_PTT                23
#define PIN_COSLED             22
#define PIN_DSTAR              9
#define PIN_DMR                8
#define PIN_YSF                7
#define PIN_P25                6
#define ADC_CHER_Chan          (1<<13)                // ADC on Due pin A11 - Due AD13 - (1 << 13) (PB20)
#define ADC_ISR_EOC_Chan       ADC_ISR_EOC13
#define ADC_CDR_Chan           13
#define DACC_MR_USER_SEL_Chan  DACC_MR_USER_SEL_CHANNEL1 // DAC on Due DAC1
#define DACC_CHER_Chan         DACC_CHER_CH1
#elif defined(ARDUINO_DUE_NTH)
#define PIN_COS                A7
#define PIN_PTT                A8
#define PIN_COSLED             A11
#define PIN_DSTAR              9
#define PIN_DMR                8
#define PIN_YSF                7
#define PIN_P25                6
#define ADC_CHER_Chan          (1<<7)                 // ADC on Due pin A0  - Due AD7 - (1 << 7)
#define ADC_ISR_EOC_Chan       ADC_ISR_EOC7
#define ADC_CDR_Chan           7
#define DACC_MR_USER_SEL_Chan  DACC_MR_USER_SEL_CHANNEL0 // DAC on Due DAC0
#define DACC_CHER_Chan         DACC_CHER_CH0
#else
#error "Either ARDUINO_DUE_PAPA, ARDUINO_DUE_ZUM_V10, or ARDUINO_DUE_NTH need to be defined"
#endif
#elif defined(__MBED__)
// A generic MBED platform
#define PIN_COS                PB_4  // D5
#define PIN_PTT                PA_8  // D7
#define PIN_COSLED             PB_10 // D6
#define PIN_ADC                PA_0  // A0
#define PIN_DAC                PA_4  // A2
#else
#error "Unknown hardware type"
#endif

extern "C" {
  void ADC_Handler()
  {
#if defined(__SAM3X8E__)
    if (ADC->ADC_ISR & ADC_ISR_EOC_Chan)          // Ensure there was an End-of-Conversion and we read the ISR reg
      io.interrupt();
#elif defined(__MBED__)
    io.interrupt();
#endif
  }
}

CIO::CIO() :
#if defined(__MBED__)
m_pinPTT(PIN_PTT),
m_pinCOSLED(PIN_COSLED),
m_pinLED(LED1),
m_pinCOS(PIN_COS),
m_pinADC(PIN_ADC),
m_pinDAC(PIN_DAC),
m_ticker(),
#endif
m_started(false),
m_rxBuffer(RX_RINGBUFFER_SIZE),
m_txBuffer(TX_RINGBUFFER_SIZE),
m_C4FSKFilter(),
m_GMSKFilter(),
m_C4FSKState(),
m_GMSKState(),
m_pttInvert(false),
m_rxLevel(128 * 128),
m_dstarTXLevel(128 * 128),
m_dmrTXLevel(128 * 128),
m_ysfTXLevel(128 * 128),
m_p25TXLevel(128 * 128),
m_ledCount(0U),
m_ledValue(true),
m_dcd(false),
m_detect(false),
m_adcOverflow(0U),
m_dacOverflow(0U),
m_count(0U),
m_watchdog(0U),
m_lockout(false)
{
  ::memset(m_C4FSKState, 0x00U, 70U * sizeof(q15_t));
  ::memset(m_GMSKState,  0x00U, 40U * sizeof(q15_t));

  m_C4FSKFilter.numTaps = C4FSK_FILTER_LEN;
  m_C4FSKFilter.pState  = m_C4FSKState;
  m_C4FSKFilter.pCoeffs = C4FSK_FILTER;

  m_GMSKFilter.numTaps = GMSK_FILTER_LEN;
  m_GMSKFilter.pState  = m_GMSKState;
  m_GMSKFilter.pCoeffs = GMSK_FILTER;

#if !defined(__MBED__)
  // Set up the TX, COS and LED pins
  pinMode(PIN_PTT,    OUTPUT);
  pinMode(PIN_COSLED, OUTPUT);
  pinMode(PIN_LED,    OUTPUT);
  pinMode(PIN_COS,    INPUT);

#if defined(ARDUINO_MODE_PINS)
  // Set up the mode output pins
  pinMode(PIN_DSTAR,  OUTPUT);
  pinMode(PIN_DMR,    OUTPUT);
  pinMode(PIN_YSF,    OUTPUT);
  pinMode(PIN_P25,    OUTPUT);
#endif
#endif
}

void CIO::start()
{
  if (m_started)
    return;

#if defined(__SAM3X8E__)
  if (ADC->ADC_ISR & ADC_ISR_EOC_Chan)        // Ensure there was an End-of-Conversion and we read the ISR reg
    io.interrupt();

  // Set up the ADC
  NVIC_EnableIRQ(ADC_IRQn);                   // Enable ADC interrupt vector
  ADC->ADC_IDR  = 0xFFFFFFFF;                 // Disable interrupts
  ADC->ADC_IER  = ADC_CHER_Chan;              // Enable End-Of-Conv interrupt
  ADC->ADC_CHDR = 0xFFFF;                     // Disable all channels
  ADC->ADC_CHER = ADC_CHER_Chan;              // Enable just one channel
  ADC->ADC_CGR  = 0x15555555;                 // All gains set to x1
  ADC->ADC_COR  = 0x00000000;                 // All offsets off
  ADC->ADC_MR   = (ADC->ADC_MR & 0xFFFFFFF0) | (1 << 1) | ADC_MR_TRGEN;  // 1 = trig source TIO from TC0

#if defined(EXTERNAL_OSC)
  // Set up the external clock input on PA4 = AI5
  REG_PIOA_ODR   = 0x10;                      // Set pin as input
  REG_PIOA_PDR   = 0x10;                      // Disable PIO A bit 4
  REG_PIOA_ABSR &= ~0x10;                     // Select A peripheral = TCLK1 Input
#endif

  // Set up the timer
  pmc_enable_periph_clk(TC_INTERFACE_ID + 0*3+0) ;  // Clock the TC0 channel 0
  TcChannel* t = &(TC0->TC_CHANNEL)[0];       // Pointer to TC0 registers for its channel 0
  t->TC_CCR = TC_CCR_CLKDIS;                  // Disable internal clocking while setup regs
  t->TC_IDR = 0xFFFFFFFF;                     // Disable interrupts
  t->TC_SR;                                   // Read int status reg to clear pending
#if defined(EXTERNAL_OSC)
  t->TC_CMR = TC_CMR_TCCLKS_XC1 |             // Use XC1 = TCLK1 external clock
#else
  t->TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK1 |    // Use TCLK1 (prescale by 2, = 42MHz)
#endif
    TC_CMR_WAVE |                             // Waveform mode
    TC_CMR_WAVSEL_UP_RC |                     // Count-up PWM using RC as threshold
    TC_CMR_EEVT_XC0 |                         // Set external events from XC0 (this setup TIOB as output)
    TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_CLEAR |
    TC_CMR_BCPB_CLEAR | TC_CMR_BCPC_CLEAR;
#if defined(EXTERNAL_OSC)
  t->TC_RC  = EXTERNAL_OSC / 24000;           // Counter resets on RC, so sets period in terms of the external clock
  t->TC_RA  = EXTERNAL_OSC / 48000;           // Roughly square wave
#else
  t->TC_RC  = 1750;                           // Counter resets on RC, so sets period in terms of 42MHz internal clock
  t->TC_RA  = 880;                            // Roughly square wave
#endif
  t->TC_CMR = (t->TC_CMR & 0xFFF0FFFF) | TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_SET;  // Set clear and set from RA and RC compares
  t->TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;    // re-enable local clocking and switch to hardware trigger source.

  // Set up the DAC
  pmc_enable_periph_clk(DACC_INTERFACE_ID);   // Start clocking DAC
  DACC->DACC_CR = DACC_CR_SWRST;              // Reset DAC
  DACC->DACC_MR =
  DACC_MR_TRGEN_EN | DACC_MR_TRGSEL(1) |      // Trigger 1 = TIO output of TC0
    DACC_MR_USER_SEL_Chan |                   // Select channel
    (24 << DACC_MR_STARTUP_Pos);              // 24 = 1536 cycles which I think is in range 23..45us since DAC clock = 42MHz
  DACC->DACC_IDR  = 0xFFFFFFFF;               // No interrupts
  DACC->DACC_CHER = DACC_CHER_Chan;           // Enable channel

  digitalWrite(PIN_PTT, m_pttInvert ? HIGH : LOW);
  digitalWrite(PIN_COSLED, LOW);
  digitalWrite(PIN_LED,    HIGH);
#elif defined(__MBED__)
  m_ticker.attach(&ADC_Handler, 1.0 / 24000.0);

  m_pinPTT.write(m_pttInvert ? 1 : 0);
  m_pinCOSLED.write(0);
  m_pinLED.write(1);
#endif

  m_count   = 0U;
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
#if defined(__MBED__)
      m_pinLED.write(m_ledValue ? 1 : 0);
#else
      digitalWrite(PIN_LED, m_ledValue ? HIGH : LOW);
#endif
    }
  } else {
    if (m_ledCount >= 240000U) {
      m_ledCount = 0U;
      m_ledValue = !m_ledValue;
#if defined(__MBED__)
      m_pinLED.write(m_ledValue ? 1 : 0);
#else
      digitalWrite(PIN_LED, m_ledValue ? HIGH : LOW);
#endif
    }
    return;
  }

#if defined(USE_COS_AS_LOCKOUT)
#if defined(__MBED__)
  m_lockout = m_pinCOS.read() == 1;
#else
  m_lockout = digitalRead(PIN_COS) == HIGH;
#endif
#endif

  // Switch off the transmitter if needed
  if (m_txBuffer.getData() == 0U && m_tx) {
    m_tx = false;
#if defined(__MBED__)
    m_pinPTT.write(m_pttInvert ? 1 : 0);
#else
    digitalWrite(PIN_PTT, m_pttInvert ? HIGH : LOW);
#endif
  }

  if (m_rxBuffer.getData() >= RX_BLOCK_SIZE) {
    q15_t   samples[RX_BLOCK_SIZE + 1U];
    uint8_t control[RX_BLOCK_SIZE + 1U];

    uint8_t blockSize = RX_BLOCK_SIZE;

    for (uint16_t i = 0U; i < RX_BLOCK_SIZE; i++) {
      uint16_t sample;
      m_rxBuffer.get(sample, control[i]);

      // Detect ADC overflow
      if (m_detect && (sample == 0U || sample == 4095U))
        m_adcOverflow++;

      q15_t res1 = q15_t(sample) - DC_OFFSET;
      q31_t res2 = res1 * m_rxLevel;
      samples[i] = q15_t(__SSAT((res2 >> 15), 16));
    }

    // Handle the case of the oscillator not being accurate enough
    if (m_sampleCount > 0U) {
      m_count += RX_BLOCK_SIZE;

      if (m_count >= m_sampleCount) {
        if (m_sampleInsert) {
          blockSize++;
          samples[RX_BLOCK_SIZE] = 0;
          for (int8_t i = RX_BLOCK_SIZE - 1; i >= 0; i--)
            control[i + 1] = control[i];
        } else {
          blockSize--;
          for (uint8_t i = 0U; i < (RX_BLOCK_SIZE - 1U); i++)
            control[i] = control[i + 1U];
        }

        m_count -= m_sampleCount;
      }
    }

    if (m_lockout)
      return;

    if (m_modemState == STATE_IDLE) {
      if (m_dstarEnable) {
        q15_t GMSKVals[RX_BLOCK_SIZE + 1U];
        ::arm_fir_fast_q15(&m_GMSKFilter, samples, GMSKVals, blockSize);

        dstarRX.samples(GMSKVals, blockSize);
      }

      if (m_dmrEnable || m_ysfEnable || m_p25Enable) {
        q15_t C4FSKVals[RX_BLOCK_SIZE + 1U];
        ::arm_fir_fast_q15(&m_C4FSKFilter, samples, C4FSKVals, blockSize);

        if (m_dmrEnable) {
          if (m_duplex)
            dmrIdleRX.samples(C4FSKVals, blockSize);
          else
            dmrDMORX.samples(C4FSKVals, blockSize);
        }

        if (m_ysfEnable)
          ysfRX.samples(C4FSKVals, blockSize);

        if (m_p25Enable)
          p25RX.samples(C4FSKVals, blockSize);
      }
    } else if (m_modemState == STATE_DSTAR) {
      if (m_dstarEnable) {
        q15_t GMSKVals[RX_BLOCK_SIZE + 1U];
        ::arm_fir_fast_q15(&m_GMSKFilter, samples, GMSKVals, blockSize);

        dstarRX.samples(GMSKVals, blockSize);
      }
    } else if (m_modemState == STATE_DMR) {
      if (m_dmrEnable) {
        q15_t C4FSKVals[RX_BLOCK_SIZE + 1U];
        ::arm_fir_fast_q15(&m_C4FSKFilter, samples, C4FSKVals, blockSize);

        if (m_duplex) {
          // If the transmitter isn't on, use the DMR idle RX to detect the wakeup CSBKs
          if (m_tx)
            dmrRX.samples(C4FSKVals, control, blockSize);
          else
            dmrIdleRX.samples(C4FSKVals, blockSize);
        } else {
          dmrDMORX.samples(C4FSKVals, blockSize);
        }
      }
    } else if (m_modemState == STATE_YSF) {
      if (m_ysfEnable) {
        q15_t C4FSKVals[RX_BLOCK_SIZE + 1U];
        ::arm_fir_fast_q15(&m_C4FSKFilter, samples, C4FSKVals, blockSize);

        ysfRX.samples(C4FSKVals, blockSize);
      }
    } else if (m_modemState == STATE_P25) {
      if (m_p25Enable) {
        q15_t C4FSKVals[RX_BLOCK_SIZE + 1U];
        ::arm_fir_fast_q15(&m_C4FSKFilter, samples, C4FSKVals, blockSize);

        p25RX.samples(C4FSKVals, blockSize);
      }
    } else if (m_modemState == STATE_DSTARCAL) {
      q15_t GMSKVals[RX_BLOCK_SIZE + 1U];
      ::arm_fir_fast_q15(&m_GMSKFilter, samples, GMSKVals, blockSize);

      calDStarRX.samples(GMSKVals, blockSize);
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
#if defined(__MBED__)
    m_pinPTT.write(m_pttInvert ? 0 : 1);
#else
    digitalWrite(PIN_PTT, m_pttInvert ? LOW : HIGH);
#endif
  }

  q15_t txLevel = 0;
  switch (mode) {
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
      txLevel = m_dstarTXLevel;
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

void CIO::interrupt()
{
  uint8_t control = MARK_NONE;
  uint16_t sample = DC_OFFSET;

  m_txBuffer.get(sample, control);

#if defined(__SAM3X8E__)
  DACC->DACC_CDR = sample;
  sample = ADC->ADC_CDR[ADC_CDR_Chan];
#elif defined(__MBED__)
  m_pinDAC.write_u16(sample);
  sample = m_pinADC.read_u16();
#endif

  m_rxBuffer.put(sample, control);

  m_watchdog++;
}

void CIO::setDecode(bool dcd)
{
  if (dcd != m_dcd)
#if defined(__MBED__)
    m_pinCOSLED.write(dcd ? 1 : 0);
#else
    digitalWrite(PIN_COSLED, dcd ? HIGH : LOW);
#endif

  m_dcd = dcd;
}

void CIO::setADCDetection(bool detect)
{
  m_detect = detect;
}

void CIO::setMode()
{
#if !defined(__MBED__)
#if defined(ARDUINO_MODE_PINS)
switch (m_modemState) {
  case STATE_DSTAR:
    digitalWrite(PIN_DSTAR, HIGH);
    digitalWrite(PIN_DMR,   LOW);
    digitalWrite(PIN_YSF,   LOW);
    digitalWrite(PIN_P25,   LOW);
    break;
  case STATE_DMR:
    digitalWrite(PIN_DSTAR, LOW);
    digitalWrite(PIN_DMR,   HIGH);
    digitalWrite(PIN_YSF,   LOW);
    digitalWrite(PIN_P25,   LOW);
    break;
  case STATE_YSF:
    digitalWrite(PIN_DSTAR, LOW);
    digitalWrite(PIN_DMR,   LOW);
    digitalWrite(PIN_YSF,   HIGH);
    digitalWrite(PIN_P25,   LOW);
    break;
  case STATE_P25:
    digitalWrite(PIN_DSTAR, LOW);
    digitalWrite(PIN_DMR,   LOW);
    digitalWrite(PIN_YSF,   LOW);
    digitalWrite(PIN_P25,   HIGH);
    break;
  default:
    digitalWrite(PIN_DSTAR, LOW);
    digitalWrite(PIN_DMR,   LOW);
    digitalWrite(PIN_YSF,   LOW);
    digitalWrite(PIN_P25,   LOW);
    break;
}
#endif
#endif
}

void CIO::setParameters(bool rxInvert, bool txInvert, bool pttInvert, uint8_t rxLevel, uint8_t dstarTXLevel, uint8_t dmrTXLevel, uint8_t ysfTXLevel, uint8_t p25TXLevel)
{
  m_pttInvert = pttInvert;

  m_rxLevel      = q15_t(rxLevel * 128);
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

#if defined(WANT_DEBUG)
  if (m_adcOverflow > 0U || m_dacOverflow > 0U)
    DEBUG3("IO: adc/dac", m_adcOverflow, m_dacOverflow);
#endif

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

#if defined(SEND_RSSI_DATA)
uint16_t CIO::getRSSIValue()
{
  return 0U;
}
#endif
