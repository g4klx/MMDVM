/*
 *   Copyright (C) 2009-2017 by Jonathan Naylor G4KLX
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
#include "DMRIdleRX.h"
#include "DMRSlotType.h"
#include "Utils.h"

const q15_t SCALING_FACTOR = 19505;      // Q15(0.60)

const uint8_t MAX_SYNC_SYMBOLS_ERRS = 2U;
const uint8_t MAX_SYNC_BYTES_ERRS   = 3U;

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])

const uint16_t NOENDPTR = 9999U;

const uint8_t CONTROL_IDLE = 0x80U;
const uint8_t CONTROL_DATA = 0x40U;

CDMRIdleRX::CDMRIdleRX() :
m_bitBuffer(),
m_buffer(),
m_bitPtr(0U),
m_dataPtr(0U),
m_endPtr(NOENDPTR),
m_maxCorr(0),
m_centre(0),
m_threshold(0),
m_colorCode(0U)
{
}

void CDMRIdleRX::reset()
{
  m_dataPtr   = 0U;
  m_bitPtr    = 0U;
  m_maxCorr   = 0;
  m_threshold = 0;
  m_centre    = 0;
  m_endPtr    = NOENDPTR;
}

void CDMRIdleRX::samples(const q15_t* samples, uint8_t length)
{
  for (uint8_t i = 0U; i < length; i++)
    processSample(samples[i]);
}

void CDMRIdleRX::processSample(q15_t sample)
{
  m_bitBuffer[m_bitPtr] <<= 1;
  if (sample < 0)
    m_bitBuffer[m_bitPtr] |= 0x01U;

  m_buffer[m_dataPtr] = sample;

  if (countBits32((m_bitBuffer[m_bitPtr] & DMR_SYNC_SYMBOLS_MASK) ^ DMR_MS_DATA_SYNC_SYMBOLS) <= MAX_SYNC_SYMBOLS_ERRS) {
    uint16_t ptr = m_dataPtr + DMR_FRAME_LENGTH_SAMPLES - DMR_SYNC_LENGTH_SAMPLES + DMR_RADIO_SYMBOL_LENGTH;
    if (ptr >= DMR_FRAME_LENGTH_SAMPLES)
      ptr -= DMR_FRAME_LENGTH_SAMPLES;

    q31_t corr = 0;
    q15_t max = -16000;
    q15_t min =  16000;

    for (uint8_t i = 0U; i < DMR_SYNC_LENGTH_SYMBOLS; i++) {
      q15_t val = m_buffer[ptr];

      if (val > max)
        max = val;
      if (val < min)
        min = val;

      switch (DMR_MS_DATA_SYNC_SYMBOLS_VALUES[i]) {
      case +3:
        corr -= (val + val + val);
        break;
      case +1:
        corr -= val;
        break;
      case -1:
        corr += val;
        break;
      default:  // -3
        corr += (val + val + val);
        break;
      }

      ptr += DMR_RADIO_SYMBOL_LENGTH;
      if (ptr >= DMR_FRAME_LENGTH_SAMPLES)
        ptr -= DMR_FRAME_LENGTH_SAMPLES;
    }

    if (corr > m_maxCorr) {
      q15_t centre = (max + min) >> 1;

      q31_t v1 = (max - centre) * SCALING_FACTOR;
      q15_t threshold = q15_t(v1 >> 15);

      uint8_t sync[DMR_SYNC_BYTES_LENGTH];

      uint16_t ptr = m_dataPtr + DMR_FRAME_LENGTH_SAMPLES - DMR_SYNC_LENGTH_SAMPLES + DMR_RADIO_SYMBOL_LENGTH;
      if (ptr >= DMR_FRAME_LENGTH_SAMPLES)
        ptr -= DMR_FRAME_LENGTH_SAMPLES;

      samplesToBits(ptr, DMR_SYNC_LENGTH_SYMBOLS, sync, 4U, centre, threshold);

      uint8_t errs = 0U;
      for (uint8_t i = 0U; i < DMR_SYNC_BYTES_LENGTH; i++)
        errs += countBits8((sync[i] & DMR_SYNC_BYTES_MASK[i]) ^ DMR_MS_DATA_SYNC_BYTES[i]);

      if (errs <= MAX_SYNC_BYTES_ERRS) {
        DEBUG3("DMRIdleRX: data sync found centre/threshold", centre, threshold);
        m_maxCorr   = corr;
        m_centre    = centre;
        m_threshold = threshold;

        m_endPtr    = m_dataPtr + DMR_SLOT_TYPE_LENGTH_SAMPLES / 2U + DMR_INFO_LENGTH_SAMPLES / 2U - 1U;
        if (m_endPtr >= DMR_FRAME_LENGTH_SAMPLES)
          m_endPtr -= DMR_FRAME_LENGTH_SAMPLES;
      }
    }
  }

  if (m_dataPtr == m_endPtr) {
    uint16_t ptr = m_endPtr + DMR_RADIO_SYMBOL_LENGTH + 1U;
    if (ptr >= DMR_FRAME_LENGTH_SAMPLES)
	  ptr -= DMR_FRAME_LENGTH_SAMPLES;

    uint8_t frame[DMR_FRAME_LENGTH_BYTES + 1U];
    samplesToBits(ptr, DMR_FRAME_LENGTH_SYMBOLS, frame, 8U, m_centre, m_threshold);

    uint8_t colorCode;
    uint8_t dataType;
    CDMRSlotType slotType;
    slotType.decode(frame + 1U, colorCode, dataType);

    if (colorCode == m_colorCode && dataType == DT_CSBK) {
      frame[0U] = CONTROL_IDLE | CONTROL_DATA | DT_CSBK;
      serial.writeDMRData(false, frame, DMR_FRAME_LENGTH_BYTES + 1U);
    }

    m_endPtr  = NOENDPTR;
    m_maxCorr = 0;
  }

  m_dataPtr++;
  if (m_dataPtr >= DMR_FRAME_LENGTH_SAMPLES)
    m_dataPtr = 0U;

  m_bitPtr++;
  if (m_bitPtr >= DMR_RADIO_SYMBOL_LENGTH)
    m_bitPtr = 0U;
}

void CDMRIdleRX::samplesToBits(uint16_t start, uint8_t count, uint8_t* buffer, uint16_t offset, q15_t centre, q15_t threshold)
{
  for (uint8_t i = 0U; i < count; i++) {
    q15_t sample = m_buffer[start] - centre;

    if (sample < -threshold) {
      WRITE_BIT1(buffer, offset, false);
      offset++;
      WRITE_BIT1(buffer, offset, true);
      offset++;
    } else if (sample < 0) {
      WRITE_BIT1(buffer, offset, false);
      offset++;
      WRITE_BIT1(buffer, offset, false);
      offset++;
    } else if (sample < threshold) {
      WRITE_BIT1(buffer, offset, true);
      offset++;
      WRITE_BIT1(buffer, offset, false);
      offset++;
    } else {
      WRITE_BIT1(buffer, offset, true);
      offset++;
      WRITE_BIT1(buffer, offset, true);
      offset++;
    }

    start += DMR_RADIO_SYMBOL_LENGTH;
    if (start >= DMR_FRAME_LENGTH_SAMPLES)
      start -= DMR_FRAME_LENGTH_SAMPLES;
  }
}

void CDMRIdleRX::setColorCode(uint8_t colorCode)
{
  m_colorCode = colorCode;
}
