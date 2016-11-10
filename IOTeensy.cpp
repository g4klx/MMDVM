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

#if defined(__MK20DX256__) || defined(__MK66FX1M0__)

// A Teensy 3.1/3.2
#if defined(__MK20DX256__)
#define PIN_LED                13
#define PIN_COS                52
#define PIN_PTT                23
#define PIN_COSLED             22
#define PIN_DSTAR              9
#define PIN_DMR                8
#define PIN_YSF                7
#define PIN_P25                6
#define PIN_ADC                5        // A0
#define PIN_RSSI               8        // A2

// A Teensy 3.6
#elif defined(__MK66FX1M0__)
#define PIN_LED                13
#define PIN_COS                52
#define PIN_PTT                23
#define PIN_COSLED             22
#define PIN_DSTAR              9
#define PIN_DMR                8
#define PIN_YSF                7
#define PIN_P25                6
#define PIN_ADC                5        // A0
#define PIN_RSSI               8        // A2
#endif

const uint16_t DC_OFFSET = 2048U;

extern "C" {
  void adc0_isr()
  {
    io.interrupt();
  }

  void adc1_isr()
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
#endif
}

#define PDB_CH0C1_TOS 0x0100
#define PDB_CH0C1_EN  0x01

void CIO::startInt()
{
  // Initialise ADC0 conversion to be triggered by the PDB
  ADC0_CFG1 = ADC_CFG1_ADIV(1) | ADC_CFG1_ADICLK(1) | ADC_CFG1_MODE(1) |
              ADC_CFG1_ADLSMP;                                        // Single-ended 12 bits, long sample time
  ADC0_CFG2 = ADC_CFG2_MUXSEL | ADC_CFG2_ADLSTS(2);                   // Select channels ADxxxb
  ADC0_SC2  = ADC_SC2_REFSEL(1) | ADC_SC2_ADTRG;                      // Voltage ref internal, hardware trigger
  ADC0_SC3  = ADC_SC3_AVGE | ADC_SC3_AVGS(0);                         // Enable averaging, 4 samples

  ADC0_SC3  = ADC_SC3_CAL;                                            // Begin calibration
  while ((ADC0_SC3 & ADC_SC3_CAL) == ADC_SC3_CAL)                     // Wait for calibration
    ;

  uint16_t sum0 = ADC0_CLPS + ADC0_CLP4 + ADC0_CLP3 + ADC0_CLP2 + ADC0_CLP1 + ADC0_CLP0;   // Plus side gain
  sum0 = (sum0 / 2U) | 0x8000U;
  ADC0_PG   = sum0;

  ADC0_SC1A = ADC_SC1_AIEN | PIN_ADC;                                 // Enable ADC interrupt, use A0
  NVIC_ENABLE_IRQ(IRQ_ADC0);

  // Setup PDB for ADC0 at 24 kHz
  SIM_SCGC6 |= SIM_SCGC6_PDB;                                         // Enable PDB clock
#if F_BUS == 60000000
  // 60 MHz for the Teensy 3.5/3.6
  PDB0_MOD   = 2500 - 1;                                              // Timer period for 60 MHz bus
#else
  // 48 MHz for the Teensy 3.1/3.2
  PDB0_MOD   = 2000 - 1;                                              // Timer period for 48 MHz bus
#endif
  PDB0_IDLY  = 0;                                                     // Interrupt delay
  PDB0_CH0C1 = PDB_CH0C1_TOS | PDB_CH0C1_EN;                          // Enable pre-trigger
  PDB0_SC    = PDB_SC_TRGSEL(15) | PDB_SC_PDBEN | PDB_SC_PDBIE |
               PDB_SC_CONT | PDB_SC_PRESCALER(7) | PDB_SC_MULT(1) |
               PDB_SC_LDOK;
  PDB0_SC   |= PDB_SC_SWTRIG;                                         // Software trigger (reset and restart counter)

#if defined(SEND_RSSI_DATA)
  // Initialise ADC1 conversion to be triggered by the PDB
  ADC1_CFG1 = ADC_CFG1_ADIV(1) | ADC_CFG1_ADICLK(1) | ADC_CFG1_MODE(1) |
              ADC_CFG1_ADLSMP;                                        // Single-ended 12 bits, long sample time
  ADC1_CFG2 = ADC_CFG2_MUXSEL | ADC_CFG2_ADLSTS(2);                   // Select channels ADxxxb
  ADC1_SC2  = ADC_SC2_REFSEL(1) | ADC_SC2_ADTRG;                      // Voltage ref internal, hardware trigger
  ADC1_SC3  = ADC_SC3_AVGE | ADC_SC3_AVGS(0);                         // Enable averaging, 4 samples

  ADC1_SC3  = ADC_SC3_CAL;                                            // Begin calibration
  while ((ADC1_SC3 & ADC_SC3_CAL) == ADC_SC3_CAL)                     // Wait for calibration
    ;

  uint16_t sum1 = ADC1_CLPS + ADC1_CLP4 + ADC1_CLP3 + ADC1_CLP2 + ADC1_CLP1 + ADC1_CLP0;   // Plus side gain
  sum1 = (sum1 / 2U) | 0x8000U;
  ADC1_PG   = sum1;

  ADC1_SC1A = ADC_SC1_AIEN | PIN_RSSI;                                // Enable ADC interrupt, use A0
  NVIC_ENABLE_IRQ(IRQ_ADC1);
#endif

  // Initialise the DAC
  SIM_SCGC2 |= SIM_SCGC2_DAC0;
  DAC0_C0    = DAC_C0_DACEN | DAC_C0_DACRFS;           // 1.2V VDDA is DACREF_2

  digitalWrite(PIN_PTT, m_pttInvert ? HIGH : LOW);
  digitalWrite(PIN_COSLED, LOW);
  digitalWrite(PIN_LED,    HIGH);
}

void CIO::interrupt()
{
  if ((ADC0_SC1A & ADC_SC1_COCO) == ADC_SC1_COCO) {
    uint8_t control = MARK_NONE;
    uint16_t sample = DC_OFFSET;

    m_txBuffer.get(sample, control);
    *(int16_t *)&(DAC0_DAT0L) = sample;

    sample = ADC0_RA;
    m_rxBuffer.put(sample, control);

#if !defined(SEND_RSSI_DATA)
    m_rssiBuffer.put(0U);
#endif

    m_watchdog++;
  }

#if defined(SEND_RSSI_DATA)
  if ((ADC1_SC1A & ADC_SC1_COCO) == ADC_SC1_COCO) {
    uint16_t rssi = ADC1_RA;
    m_rssiBuffer.put(rssi);
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

