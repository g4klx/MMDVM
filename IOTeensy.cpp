/*
 *   Copyright (C) 2016 by Jonathan Naylor G4KLX
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

#if defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)

#if defined(EXTERNAL_OSC)
#define PIN_LED                3
#else
#define PIN_LED                13
#endif
#define PIN_COS                4
#define PIN_PTT                5
#define PIN_COSLED             6
#define PIN_DSTAR              9
#define PIN_DMR                10
#define PIN_YSF                11
#define PIN_P25                12
#define PIN_ADC                5        // A0
#define PIN_RSSI               8        // A2

#define PDB_CHnC1_TOS 0x0100
#define PDB_CHnC1_EN  0x0001

const uint16_t DC_OFFSET = 2048U;

extern "C" {
  void adc0_isr()
  {
    io.interrupt(0U);
  }

#if defined(SEND_RSSI_DATA)
  void adc1_isr()
  {
    io.interrupt(1U);
  }
#endif
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
#endif
}

void CIO::startInt()
{
  if ((ADC0_SC1A & ADC_SC1_COCO) == ADC_SC1_COCO)
    io.interrupt(0U);

#if defined(SEND_RSSI_DATA)
  if ((ADC1_SC1A & ADC_SC1_COCO) == ADC_SC1_COCO)
    io.interrupt(1U);
#endif

  // Initialise ADC0
  SIM_SCGC6 |= SIM_SCGC6_ADC0;
  ADC0_CFG1  = ADC_CFG1_ADIV(1) | ADC_CFG1_ADICLK(1) |                // Single-ended 12 bits, long sample time
               ADC_CFG1_MODE(1) | ADC_CFG1_ADLSMP;
  ADC0_CFG2  = ADC_CFG2_MUXSEL | ADC_CFG2_ADLSTS(2);                  // Select channels ADxxxb
  ADC0_SC2   = ADC_SC2_REFSEL(1) | ADC_SC2_ADTRG;                     // Voltage ref internal, hardware trigger
  ADC0_SC3   = ADC_SC3_CAL | ADC_SC3_AVGE | ADC_SC3_AVGS(0);          // Enable averaging, 4 samples

  while ((ADC0_SC3 & ADC_SC3_CAL) == ADC_SC3_CAL)                     // Wait for calibration
    ;

  uint16_t sum0 = ADC0_CLPS + ADC0_CLP4 + ADC0_CLP3 +                 // Plus side gain
                  ADC0_CLP2 + ADC0_CLP1 + ADC0_CLP0;
  sum0 = (sum0 / 2U) | 0x8000U;
  ADC0_PG    = sum0;

  ADC0_SC1A  = ADC_SC1_AIEN | PIN_ADC;                                // Enable ADC interrupt, use A0
  NVIC_ENABLE_IRQ(IRQ_ADC0);

#if defined(SEND_RSSI_DATA)
  // Initialise ADC1
  SIM_SCGC3 |= SIM_SCGC3_ADC1;
  ADC1_CFG1  = ADC_CFG1_ADIV(1) | ADC_CFG1_ADICLK(1) |                // Single-ended 12 bits, long sample time
               ADC_CFG1_MODE(1) | ADC_CFG1_ADLSMP;
  ADC1_CFG2  = ADC_CFG2_MUXSEL | ADC_CFG2_ADLSTS(2);                  // Select channels ADxxxb
  ADC1_SC2   = ADC_SC2_REFSEL(1) | ADC_SC2_ADTRG;                     // Voltage ref internal, hardware trigger
  ADC1_SC3   = ADC_SC3_CAL | ADC_SC3_AVGE | ADC_SC3_AVGS(0);          // Enable averaging, 4 samples

  while ((ADC1_SC3 & ADC_SC3_CAL) == ADC_SC3_CAL)                     // Wait for calibration
    ;

  uint16_t sum1 = ADC1_CLPS + ADC1_CLP4 + ADC1_CLP3 +                 // Plus side gain
                  ADC1_CLP2 + ADC1_CLP1 + ADC1_CLP0;
  sum1 = (sum1 / 2U) | 0x8000U;
  ADC1_PG    = sum1;

  ADC1_SC1A  = ADC_SC1_AIEN | PIN_RSSI;                               // Enable ADC interrupt, use A2
  NVIC_ENABLE_IRQ(IRQ_ADC1);
#endif

#if defined(EXTERNAL_OSC)
  SIM_SCGC5  |= SIM_SCGC5_LPTIMER;
  LPTMR0_PSR  = LPTMR_PSR_PBYP;                                       // Bypass prescaler/filter
  LPTMR0_CMR  = EXTERNAL_OSC / 24000;
  LPTMR0_CSR  = LPTMR_CSR_TIE | LPTMR_CSR_TPS(2) |                    // Interrupt, counter, input=Alt2, free running mode, enable
                LPTMR_CSR_TFC | LPTMR_CSR_TMS |
                LPTMR_CSR_TEN;

  CORE_PIN13_CONFIG = PORT_PCR_MUX(3);

  // Set ADC0 to trigger from the LPTMR
  SIM_SOPT7  |= SIM_SOPT7_ADC0ALTTRGEN |
               !SIM_SOPT7_ADC0PRETRGSEL |
                SIM_SOPT7_ADC0TRGSEL(14);
#if defined(SEND_RSSI_DATA)
  // Set ADC1 to trigger from the LPTMR
  SIM_SOPT7  |= SIM_SOPT7_ADC1ALTTRGEN |
               !SIM_SOPT7_ADC1PRETRGSEL |
                SIM_SOPT7_ADC1TRGSEL(14);
#endif
  NVIC_ENABLE_IRQ(IRQ_LPTMR);
#else
  // Setup PDB for ADC0 (and ADC1) at 24 kHz
  SIM_SCGC6  |= SIM_SCGC6_PDB;                                        // Enable PDB clock
  PDB0_MOD    = F_BUS / 24000;                                        // Timer period
  PDB0_IDLY   = 0;                                                    // Interrupt delay
  PDB0_CH0C1  = PDB_CHnC1_TOS | PDB_CHnC1_EN;                         // Enable pre-trigger for ADC0
#if defined(SEND_RSSI_DATA)
  PDB0_CH1C1  = PDB_CHnC1_TOS | PDB_CHnC1_EN;                         // Enable pre-trigger for ADC1
#endif
  PDB0_SC     = PDB_SC_TRGSEL(15) | PDB_SC_PDBEN |                    // SW trigger, enable interrupts, continuous mode
                PDB_SC_PDBIE | PDB_SC_CONT | PDB_SC_LDOK;             // No prescaling
  PDB0_SC    |= PDB_SC_SWTRIG;                                        // Software trigger (reset and restart counter)
  NVIC_ENABLE_IRQ(IRQ_PDB);
#endif

  // Initialise the DAC
  SIM_SCGC2 |= SIM_SCGC2_DAC0;
  DAC0_C0    = DAC_C0_DACEN | DAC_C0_DACRFS;           // 1.2V VDDA is DACREF_2

  digitalWrite(PIN_PTT, m_pttInvert ? HIGH : LOW);
  digitalWrite(PIN_COSLED, LOW);
  digitalWrite(PIN_LED,    HIGH);
}

void CIO::interrupt(uint8_t source)
{
  if (source == 0U) {  // ADC0
    uint8_t control = MARK_NONE;
    uint16_t sample = DC_OFFSET;

    m_txBuffer.get(sample, control);
    *(int16_t *)&(DAC0_DAT0L) = sample;

    if ((ADC0_SC1A & ADC_SC1_COCO) == ADC_SC1_COCO) {
      sample = ADC0_RA;
      m_rxBuffer.put(sample, control);

#if !defined(SEND_RSSI_DATA)
      m_rssiBuffer.put(0U);
#endif
    }

    m_watchdog++;
  }

#if defined(SEND_RSSI_DATA)
  if (source == 1U) {  // ADC1
    if ((ADC1_SC1A & ADC_SC1_COCO) == ADC_SC1_COCO) {
      uint16_t rssi = ADC1_RA;
      m_rssiBuffer.put(rssi);
    }
  }
#endif
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

#endif
