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
#endif
}

void CIO::startInt()
{
  // Initialise ADC1 conversion to be triggered by the PDB

  // Setup interrupt on ADC1 conversion finished

  // Setup PDB for ADC1 at 24 kHz


#if defined(SEND_RSSI_DATA)
  // Initialise ADC2 conversion to be triggered by the PDB

  // Setup interrupt on ADC2 conversion finished

  // Setup PDB for ADC2 at 24 kHz
#endif

  // Initialise the DAC
  SIM_SCGC2 |= SIM_SCGC2_DAC0;
  DAC0_C0 = DAC_C0_DACEN;                   // 1.2V VDDA is DACREF_2

  digitalWrite(PIN_PTT, m_pttInvert ? HIGH : LOW);
  digitalWrite(PIN_COSLED, LOW);
  digitalWrite(PIN_LED,    HIGH);
}

void CIO::interrupt()
{
  uint8_t control = MARK_NONE;
  uint16_t sample = DC_OFFSET;

  m_txBuffer.get(sample, control);

  DAC0_DAT0L = (sample >> 0) & 0xFFU;
  DAC0_DATH  = (sample >> 8) & 0xFFU;

  // sample = 

  m_rxBuffer.put(sample, control);
  m_rssiBuffer.put(0U);

  m_watchdog++;
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

