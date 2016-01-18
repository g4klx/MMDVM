/*
 *   Copyright (C) 2015,2016 by Jonathan Naylor G4KLX
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

// Generated using rcosdesign(0.2, 8, 5, 'sqrt') in MATLAB
static q15_t   C4FSK_FILTER[] = {401, 104, -340, -731, -847, -553, 112, 909, 1472, 1450, 683, -675, -2144, -3040, -2706, -770, 2667, 6995,
                                 11237, 14331, 15464, 14331, 11237, 6995, 2667, -770, -2706, -3040, -2144, -675, 683, 1450, 1472, 909, 112,
                                 -553, -847, -731, -340, 104, 401, 0};
const uint16_t C4FSK_FILTER_LEN = 42U;

// Generated using gaussfir(0.5, 4, 5) in MATLAB
static q15_t   GMSK_FILTER[] = {8, 104, 760, 3158, 7421, 9866, 7421, 3158, 760, 104, 8, 0};
const uint16_t GMSK_FILTER_LEN = 12U;

const uint16_t DC_OFFSET = 2048U;

const uint16_t TX_BUFFER_SIZE = 501U;
const uint16_t RX_BUFFER_SIZE = 601U;

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
#elif defined(ARDUINO_DUE_ZUM)
#define PIN_COS                52
#define PIN_PTT                23
#define PIN_COSLED             22
#define ADC_CHER_Chan          (1<<13)                // ADC on Due pin A11 - Due AD13 - (1 << 13) (PB20)
#define ADC_ISR_EOC_Chan       ADC_ISR_EOC13
#define ADC_CDR_Chan           13
#define DACC_MR_USER_SEL_Chan  DACC_MR_USER_SEL_CHANNEL1 // DAC on Due DAC1
#define DACC_CHER_Chan         DACC_CHER_CH1
#elif defined(ARDUINO_DUE_NTH)
#define PIN_COS                A7
#define PIN_PTT                A8
#define PIN_COSLED             A11
#define ADC_CHER_Chan          (1<<7)                 // ADC on Due pin A0  - Due AD7 - (1 << 7)
#define ADC_ISR_EOC_Chan       ADC_ISR_EOC7
#define ADC_CDR_Chan           7
#define DACC_MR_USER_SEL_Chan  DACC_MR_USER_SEL_CHANNEL0 // DAC on Due DAC0
#define DACC_CHER_Chan         DACC_CHER_CH0
#else
#error "Either ARDUINO_DUE_PAPA, ARDUINO_DUE_ZUM, or ARDUINO_DUE_NTH need to be defined"
#endif
#elif defined(__MK20DX256__)
// A Teensy 3.1/3.2
#define PIN_COS                14
#define PIN_PTT                15
#define PIN_COSLED             13
#elif defined(__MBED__)
// A generic MBED platform
#define PIN_COS                PC_1
#define PIN_PTT                PA_1
#define PIN_COSLED             PB_0
#define PIN_ADC                PA_0
#define PIN_DAC                PA_2
#else
#error "Unknown hardware type"
#endif

extern "C" {
  void ADC_Handler()
  {
#if defined(__SAM3X8E__)
    if (ADC->ADC_ISR & ADC_ISR_EOC_Chan)          // Ensure there was an End-of-Conversion and we read the ISR reg
      io.interrupt();
#elif defined(__MK20DX256__)
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
m_pinADC(PIN_ADC),
m_pinDAC(PIN_DAC),
m_ticker(),
#endif
m_started(false),
m_rxBuffer(RX_BUFFER_SIZE),
m_txBuffer(TX_BUFFER_SIZE),
m_C4FSKFilter(),
m_GMSKFilter(),
m_C4FSKState(),
m_GMSKState(),
m_pttInvert(false),
m_rxLevel(128 * 128),
m_txLevel(128 * 128),
m_ledCount(0U),
m_ledValue(true),
m_dcd(false),
m_overflow(0U),
m_overcount(0U)
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

  // Set up the timer
  pmc_enable_periph_clk(TC_INTERFACE_ID + 0*3+0) ;  // Clock the TC0 channel 0
  TcChannel* t = &(TC0->TC_CHANNEL)[0];       // Pointer to TC0 registers for its channel 0
  t->TC_CCR = TC_CCR_CLKDIS;                  // Disable internal clocking while setup regs
  t->TC_IDR = 0xFFFFFFFF;                     // Disable interrupts
  t->TC_SR;                                   // Read int status reg to clear pending
  t->TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK1 |    // Use TCLK1 (prescale by 2, = 42MHz)
              TC_CMR_WAVE |                   // Waveform mode
              TC_CMR_WAVSEL_UP_RC |           // Count-up PWM using RC as threshold
              TC_CMR_EEVT_XC0 |               // Set external events from XC0 (this setup TIOB as output)
              TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_CLEAR |
              TC_CMR_BCPB_CLEAR | TC_CMR_BCPC_CLEAR;
  t->TC_RC  = 1750;                           // Counter resets on RC, so sets period in terms of 42MHz clock (was 875)
  t->TC_RA  = 880;                            // Roughly square wave (was 440)
  t->TC_CMR = (t->TC_CMR & 0xFFF0FFFF) | TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_SET;  // Set clear and set from RA and RC compares
  t->TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG ;  // re-enable local clocking and switch to hardware trigger source.

  // Set up the DAC
  pmc_enable_periph_clk(DACC_INTERFACE_ID);   // Start clocking DAC
  DACC->DACC_CR = DACC_CR_SWRST;              // Reset DAC
  DACC->DACC_MR =
    DACC_MR_TRGEN_EN | DACC_MR_TRGSEL(1) |    // Trigger 1 = TIO output of TC0
    DACC_MR_USER_SEL_Chan |                   // Select channel
    (24 << DACC_MR_STARTUP_Pos);              // 24 = 1536 cycles which I think is in range 23..45us since DAC clock = 42MHz
  DACC->DACC_IDR  = 0xFFFFFFFF;               // No interrupts
  DACC->DACC_CHER = DACC_CHER_Chan;           // Enable channel

  digitalWrite(PIN_PTT, m_pttInvert ? HIGH : LOW);
  digitalWrite(PIN_COSLED, LOW);
  digitalWrite(PIN_LED, HIGH);
#elif defined(__MK20DX256__)
  digitalWrite(PIN_PTT, m_pttInvert ? HIGH : LOW);
  digitalWrite(PIN_COSLED, LOW);
  digitalWrite(PIN_LED, HIGH);
#elif defined(__MBED__)
  m_ticker.attach(&ADC_Handler, 1.0 / 24000.0);

  m_pinPTT.write(m_pttInvert ? 1 : 0);
  m_pinCOSLED.write(0);
  m_pinLED.write(1);
#endif

  m_started = true;
}

void CIO::process()
{
  m_ledCount++;
  if (m_started) {
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
    q15_t   samples[RX_BLOCK_SIZE];
    uint8_t control[RX_BLOCK_SIZE];

    for (uint16_t i = 0U; i < RX_BLOCK_SIZE; i++) {
      uint16_t sample;
      m_rxBuffer.get(sample, control[i]);

      // Detect ADC overflow
      if (m_dcd && (sample == 0U || sample == 4095U))
        m_overflow++;
      m_overcount++;

      q15_t res1 = q15_t(sample) - DC_OFFSET;
      q31_t res2 = res1 * m_rxLevel;
      samples[i] = q15_t(__SSAT((res2 >> 15), 16));
    }

    if (m_modemState == STATE_IDLE) {
      if (m_dstarEnable) {
        q15_t GMSKVals[RX_BLOCK_SIZE];
        ::arm_fir_fast_q15(&m_GMSKFilter, samples, GMSKVals, RX_BLOCK_SIZE);

        dstarRX.samples(GMSKVals, RX_BLOCK_SIZE);
      }

      if (m_dmrEnable || m_ysfEnable) {
        q15_t C4FSKVals[RX_BLOCK_SIZE];
        ::arm_fir_fast_q15(&m_C4FSKFilter, samples, C4FSKVals, RX_BLOCK_SIZE);

        if (m_dmrEnable)
          dmrIdleRX.samples(C4FSKVals, RX_BLOCK_SIZE);

        if (m_ysfEnable)
          ysfRX.samples(C4FSKVals, RX_BLOCK_SIZE);
      }
    } else if (m_modemState == STATE_DSTAR) {
      if (m_dstarEnable) {
        q15_t GMSKVals[RX_BLOCK_SIZE];
        ::arm_fir_fast_q15(&m_GMSKFilter, samples, GMSKVals, RX_BLOCK_SIZE);

        dstarRX.samples(GMSKVals, RX_BLOCK_SIZE);
      }
    } else if (m_modemState == STATE_DMR) {
      if (m_dmrEnable) {
        q15_t C4FSKVals[RX_BLOCK_SIZE];
        ::arm_fir_fast_q15(&m_C4FSKFilter, samples, C4FSKVals, RX_BLOCK_SIZE);

        // If the transmitter isn't on, use the DMR idle RX to detect the wakeup CSBKs
        if (m_tx)
          dmrRX.samples(C4FSKVals, control, RX_BLOCK_SIZE);
        else
          dmrIdleRX.samples(C4FSKVals, RX_BLOCK_SIZE);
      }
    } else if (m_modemState == STATE_YSF) {
      if (m_ysfEnable) {
        q15_t C4FSKVals[RX_BLOCK_SIZE];
        ::arm_fir_fast_q15(&m_C4FSKFilter, samples, C4FSKVals, RX_BLOCK_SIZE);

        ysfRX.samples(C4FSKVals, RX_BLOCK_SIZE);
      }
    } else if (m_modemState == STATE_CALIBRATE) {
      q15_t GMSKVals[RX_BLOCK_SIZE];
      ::arm_fir_fast_q15(&m_GMSKFilter, samples, GMSKVals, RX_BLOCK_SIZE);

      calRX.samples(GMSKVals, RX_BLOCK_SIZE);
    }
  }
}

void CIO::write(q15_t* samples, uint16_t length, const uint8_t* control)
{
  if (!m_started)
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

  for (uint16_t i = 0U; i < length; i++) {
    q31_t res1 = samples[i] * m_txLevel;
    q15_t res2 = q15_t(__SSAT((res1 >> 15), 16));
    
    if (control == NULL)
      m_txBuffer.put(uint16_t(res2 + DC_OFFSET), MARK_NONE);
    else
      m_txBuffer.put(uint16_t(res2 + DC_OFFSET), control[i]);
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
#elif defined(__MK20DX256__)
#elif defined(__MBED__)
  m_pinDAC.write_u16(sample);
  sample = m_pinADC.read_u16();
#endif

  m_rxBuffer.put(sample, control);
}

void CIO::setDecode(bool dcd)
{
  if (dcd != m_dcd)
#if defined(__MBED__)
    m_pinCOSLED.write(dcd ? 1 : 0);
#elif defined(PIN_COSLED)
    digitalWrite(PIN_COSLED, dcd ? HIGH : LOW);
#endif

  m_dcd = dcd;
}

void CIO::setParameters(bool rxInvert, bool txInvert, bool pttInvert, uint8_t rxLevel, uint8_t txLevel)
{
  m_pttInvert = pttInvert;

  m_rxLevel = q15_t(rxLevel * 128);
  m_txLevel = q15_t(txLevel * 128);
  
  if (rxInvert)
    m_rxLevel = -m_rxLevel;
  
  if (txInvert)
    m_txLevel = -m_txLevel;
}

bool CIO::hasADCOverflow()
{
  bool overflow = m_overflow > 0U;

#if defined(WANT_DEBUG)
  if (m_overflow > 0U)
    DEBUG3("IO: Overflow, n/count", m_overflow, m_overcount);
#endif

  m_overflow = 0U;
  m_overcount = 0U;
  
  return overflow;
}

bool CIO::hasTXOverflow()
{
  return m_txBuffer.hasOverflowed();
}

bool CIO::hasRXOverflow()
{
  return m_rxBuffer.hasOverflowed();
}

