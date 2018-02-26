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

#if defined(__SAM3X8E__)

// An Arduino Due
#if defined(ARDUINO_DUE_PAPA)
#define PIN_COS                7
#define PIN_PTT                8
#define PIN_COSLED             11
#define PIN_DSTAR              16
#define PIN_DMR                17
#define PIN_YSF                18
#define PIN_P25                19
#define PIN_NXDN               20
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
#define PIN_NXDN               5
#define ADC_CHER_Chan          (1<<13)                // ADC on Due pin A11 - Due AD13 - (1 << 13)
#define ADC_ISR_EOC_Chan       ADC_ISR_EOC13
#define ADC_CDR_Chan           13
#define DACC_MR_USER_SEL_Chan  DACC_MR_USER_SEL_CHANNEL1 // DAC on Due DAC1
#define DACC_CHER_Chan         DACC_CHER_CH1
#define RSSI_CHER_Chan         (1<<1)                 // ADC on Due pin A6  - Due AD1 - (1 << 1)
#define RSSI_CDR_Chan          1
#elif defined(ARDUINO_DUE_NTH)
#define PIN_COS                A7
#define PIN_PTT                A8
#define PIN_COSLED             A11
#define PIN_DSTAR              9
#define PIN_DMR                8
#define PIN_YSF                7
#define PIN_P25                6
#define PIN_NXDN               5
#define ADC_CHER_Chan          (1<<7)                 // ADC on Due pin A0  - Due AD7 - (1 << 7)
#define ADC_ISR_EOC_Chan       ADC_ISR_EOC7
#define ADC_CDR_Chan           7
#define DACC_MR_USER_SEL_Chan  DACC_MR_USER_SEL_CHANNEL0 // DAC on Due DAC0
#define DACC_CHER_Chan         DACC_CHER_CH0
#define RSSI_CHER_Chan         (1<<1)                 // ADC on Due pin A6  - Due AD1 - (1 << 1)
#define RSSI_CDR_Chan          1
#else
#error "Either ARDUINO_DUE_PAPA, ARDUINO_DUE_ZUM_V10, or ARDUINO_DUE_NTH need to be defined"
#endif

const uint16_t DC_OFFSET = 2048U;

extern "C" {
  void ADC_Handler()
  {
    io.interrupt();
  }
}

void CIO::initInt()
{
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
  pinMode(PIN_NXDN,   OUTPUT);
#endif
}

void CIO::startInt()
{
  if (ADC->ADC_ISR & ADC_ISR_EOC_Chan)        // Ensure there was an End-of-Conversion and we read the ISR reg
    io.interrupt();

  // Set up the ADC
  NVIC_EnableIRQ(ADC_IRQn);                   // Enable ADC interrupt vector
  ADC->ADC_IDR  = 0xFFFFFFFF;                 // Disable interrupts
  ADC->ADC_IER  = ADC_CHER_Chan;              // Enable End-Of-Conv interrupt
  ADC->ADC_CHDR = 0xFFFF;                     // Disable all channels
  ADC->ADC_CHER = ADC_CHER_Chan;              // Enable rx input channel
#if defined(SEND_RSSI_DATA)
  ADC->ADC_CHER |= RSSI_CHER_Chan;            // and RSSI input
#endif
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
    DACC_MR_TRGEN_EN | DACC_MR_TRGSEL(1) |    // Trigger 1 = TIO output of TC0
    DACC_MR_USER_SEL_Chan |                   // Select channel
    (24 << DACC_MR_STARTUP_Pos);              // 24 = 1536 cycles which I think is in range 23..45us since DAC clock = 42MHz
  DACC->DACC_IDR  = 0xFFFFFFFF;               // No interrupts
  DACC->DACC_CHER = DACC_CHER_Chan;           // Enable channel

  digitalWrite(PIN_PTT, m_pttInvert ? HIGH : LOW);
  digitalWrite(PIN_COSLED, LOW);
  digitalWrite(PIN_LED,    HIGH);
}

void CIO::interrupt()
{
  if ((ADC->ADC_ISR & ADC_ISR_EOC_Chan) == ADC_ISR_EOC_Chan) {    // Ensure there was an End-of-Conversion and we read the ISR reg
    uint8_t control = MARK_NONE;
    uint16_t sample = DC_OFFSET;

    m_txBuffer.get(sample, control);
    DACC->DACC_CDR = sample;

    sample = ADC->ADC_CDR[ADC_CDR_Chan];
    m_rxBuffer.put(sample, control);

#if defined(SEND_RSSI_DATA)
    m_rssiBuffer.put(ADC->ADC_CDR[RSSI_CDR_Chan]);
#else
    m_rssiBuffer.put(0U);
#endif

    m_watchdog++;
  }
}

bool CIO::getCOSInt()
{
  return digitalRead(PIN_COS) == HIGH;
}

void CIO::setLEDInt(bool on)
{
  digitalWrite(PIN_LED, on ? HIGH : LOW);
}

void CIO::setPTTInt(bool on)
{
  digitalWrite(PIN_PTT, on ? HIGH : LOW);
}

void CIO::setCOSInt(bool on)
{
  digitalWrite(PIN_COSLED, on ? HIGH : LOW);
}

void CIO::setDStarInt(bool on)
{
  digitalWrite(PIN_DSTAR, on ? HIGH : LOW);
}

void CIO::setDMRInt(bool on)
{
  digitalWrite(PIN_DMR, on ? HIGH : LOW);
}

void CIO::setYSFInt(bool on)
{
  digitalWrite(PIN_YSF, on ? HIGH : LOW);
}

void CIO::setP25Int(bool on)
{
  digitalWrite(PIN_P25, on ? HIGH : LOW);
}

void CIO::setNXDNInt(bool on)
{
  digitalWrite(PIN_NXDN, on ? HIGH : LOW);
}

void CIO::delayInt(unsigned int dly)
{
  delay(dly);
}

#endif

