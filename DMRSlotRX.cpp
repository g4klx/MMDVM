/*
 *   Copyright (C) 2009-2016 by Jonathan Naylor G4KLX
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
#include "DMRSlotRX.h"
#include "DMRSlotType.h"
#include "Utils.h"

const q15_t SCALING_FACTOR = 19505;      // Q15(0.60)

const uint8_t MAX_SYNC_SYMBOLS_ERRS = 2U;
const uint8_t MAX_SYNC_BYTES_ERRS   = 3U;

const uint8_t MAX_SYNC_LOST_FRAMES  = 13U;

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])

const uint16_t NOENDPTR = 9999U;

CDMRSlotRX::CDMRSlotRX(bool slot) :
m_slot(slot),
m_bitBuffer(),
m_buffer(),
m_bitPtr(0U),
m_dataPtr(0U),
m_syncPtr(0U),
m_endPtr(NOENDPTR),
m_maxCorr(0),
m_centre(0),
m_threshold(0),
m_control(0x00U),
m_syncCount(0U),
m_colorCode(0U),
m_n(0U)
{
}

void CDMRSlotRX::start()
{
  m_dataPtr = 0U;
  m_bitPtr  = 0U;
  m_maxCorr = 0;
  m_control = 0x00U;
}

void CDMRSlotRX::reset()
{
  m_syncPtr   = 0U;
  m_dataPtr   = 0U;
  m_bitPtr    = 0U;
  m_maxCorr   = 0;
  m_control   = 0x00U;
  m_syncCount = 0U;
  m_threshold = 0;
  m_centre    = 0;
  m_endPtr    = NOENDPTR;
}

bool CDMRSlotRX::processSample(q15_t sample)
{
  // Ensure that the buffer doesn't overflow
  if (m_dataPtr > m_endPtr || m_dataPtr >= 900U)
    return m_endPtr != NOENDPTR;

  m_buffer[m_dataPtr] = sample;

  m_bitBuffer[m_bitPtr] <<= 1;
  if (sample < 0)
    m_bitBuffer[m_bitPtr] |= 0x01U;

  // The approximate position of the sync samples
  if (m_endPtr != NOENDPTR) {
    uint16_t min = m_syncPtr - 5U;
    uint16_t max = m_syncPtr + 5U;
    if (m_dataPtr >= min && m_dataPtr <= max)
      correlateSync(sample);
  } else {
    if (m_dataPtr >= 380U && m_dataPtr <= 530U)
      correlateSync(sample);
  }

  if (m_dataPtr == m_endPtr) {
    uint8_t frame[DMR_FRAME_LENGTH_BYTES + 1U];
    frame[0U] = m_control;

    uint16_t ptr = m_endPtr - DMR_FRAME_LENGTH_SAMPLES + DMR_RADIO_SYMBOL_LENGTH + 1U;
    samplesToBits(ptr, DMR_FRAME_LENGTH_SYMBOLS, frame, 8U, m_centre, m_threshold);

    if (m_control == 0x40U) {
      // Data sync
      uint8_t colorCode;
      uint8_t dataType;
      CDMRSlotType slotType;
      slotType.decode(frame + 1U, colorCode, dataType);

      if (colorCode == m_colorCode) {
        m_syncCount = 0U;
        m_n         = 0U;

        frame[0U] |= dataType;

        switch (dataType) {
          case DT_DATA_HEADER:
          case DT_VOICE_LC_HEADER:
          case DT_VOICE_PI_HEADER:
            DEBUG4("DMRSlotRX: header for slot/color code/data type", m_slot ? 2U : 1U, colorCode, dataType);
            serial.writeDMRData(m_slot, frame, DMR_FRAME_LENGTH_BYTES + 1U);
            break;
          case DT_TERMINATOR_WITH_LC:
            DEBUG3("DMRSlotRX: terminator for slot/color code", m_slot ? 2U : 1U, colorCode);
            m_endPtr = NOENDPTR;
            serial.writeDMRData(m_slot, frame, DMR_FRAME_LENGTH_BYTES + 1U);
            break;
          default:
            DEBUG4("DMRSlotRX: data sync for slot/color code/data type", m_slot ? 2U : 1U, colorCode, dataType);
            serial.writeDMRData(m_slot, frame, DMR_FRAME_LENGTH_BYTES + 1U);
            break;
        }
      }
    } else if (m_control == 0x20U) {
      // Voice sync
      DEBUG2("DMRSlotRX: voice sync for slot", m_slot ? 2U : 1U);
      serial.writeDMRData(m_slot, frame, DMR_FRAME_LENGTH_BYTES + 1U);
      m_syncCount = 0U;
      m_n         = 0U;
    } else {
      m_syncCount++;
      if (m_syncCount >= MAX_SYNC_LOST_FRAMES) {
        DEBUG2("DMRSlotRX: lost for slot", m_slot ? 2U : 1U);
        serial.writeDMRLost(m_slot);
        m_syncCount = 0U;
        m_threshold = 0;
        m_centre    = 0;
        m_endPtr    = NOENDPTR;
        return false;
      }

      // Voice data
      frame[0U] |= ++m_n;
      serial.writeDMRData(m_slot, frame, DMR_FRAME_LENGTH_BYTES + 1U);
    }
  }

  m_dataPtr++;

  m_bitPtr++;
  if (m_bitPtr >= DMR_RADIO_SYMBOL_LENGTH)
    m_bitPtr = 0U;

  return m_endPtr != NOENDPTR;
}

void CDMRSlotRX::correlateSync(q15_t sample)
{
  uint8_t errs = countBits32((m_bitBuffer[m_bitPtr] & DMR_SYNC_SYMBOLS_MASK) ^ DMR_MS_DATA_SYNC_SYMBOLS);

  // The voice sync is the complement of the data sync
  bool data  = (errs <= MAX_SYNC_SYMBOLS_ERRS);
  bool voice = (errs >= (DMR_SYNC_LENGTH_SYMBOLS - MAX_SYNC_SYMBOLS_ERRS));

  if (data || voice) {
    uint16_t ptr = m_dataPtr - DMR_SYNC_LENGTH_SAMPLES + DMR_RADIO_SYMBOL_LENGTH;

    q31_t corr = 0;
    q15_t min =  16000;
    q15_t max = -16000;

    uint32_t mask = 0x00800000U;
    for (uint8_t i = 0U; i < DMR_SYNC_LENGTH_SYMBOLS; i++, mask >>= 1) {
      bool b = (DMR_MS_DATA_SYNC_SYMBOLS & mask) == mask;

      if (m_buffer[ptr] > max)
        max = m_buffer[ptr];
      if (m_buffer[ptr] < min)
        min = m_buffer[ptr];

      if (data)
        corr += b ? -m_buffer[ptr] : m_buffer[ptr];
      else  // if (voice)
        corr += b ? m_buffer[ptr] : -m_buffer[ptr];

      ptr += DMR_RADIO_SYMBOL_LENGTH;
    }

    if (corr > m_maxCorr) {
      q15_t centre = (max + min) >> 1;

      q31_t v1 = (max - centre) * SCALING_FACTOR;
      q15_t threshold = q15_t(v1 >> 15);

      uint8_t sync[DMR_SYNC_BYTES_LENGTH];
      uint16_t ptr = m_dataPtr - DMR_SYNC_LENGTH_SAMPLES + DMR_RADIO_SYMBOL_LENGTH;
      samplesToBits(ptr, DMR_SYNC_LENGTH_SYMBOLS, sync, 4U, centre, threshold);

      if (data) {
        uint8_t errs = 0U;
        for (uint8_t i = 0U; i < DMR_SYNC_BYTES_LENGTH; i++)
          errs += countBits8((sync[i] & DMR_SYNC_BYTES_MASK[i]) ^ DMR_MS_DATA_SYNC_BYTES[i]);

        if (errs <= MAX_SYNC_BYTES_ERRS) {
          DEBUG5("DMRSlotRX: data sync found slot/pos/centre/threshold", m_slot ? 2U : 1U, m_dataPtr, centre, threshold);
          m_maxCorr   = corr;
          m_centre    = centre;
          m_threshold = threshold;
          m_control   = 0x40U;
          m_syncPtr   = m_dataPtr;
          m_endPtr    = m_dataPtr + DMR_SLOT_TYPE_LENGTH_SAMPLES / 2U + DMR_INFO_LENGTH_SAMPLES / 2U - 1U;
          DEBUG3("DMRSlotRX: slot/endPtr", m_slot ? 2U : 1U, m_endPtr);
        }
      } else {  // if (voice)
        uint8_t errs = 0U;
        for (uint8_t i = 0U; i < DMR_SYNC_BYTES_LENGTH; i++)
          errs += countBits8((sync[i] & DMR_SYNC_BYTES_MASK[i]) ^ DMR_MS_VOICE_SYNC_BYTES[i]);

        if (errs <= MAX_SYNC_BYTES_ERRS) {
          DEBUG5("DMRSlotRX: voice sync found slot/pos/centre/threshold", m_slot ? 2U : 1U, m_dataPtr, centre, threshold);
          m_maxCorr   = corr;
          m_centre    = centre;
          m_threshold = threshold;
          m_control   = 0x20U;
          m_syncPtr   = m_dataPtr;
          m_endPtr    = m_dataPtr + DMR_SLOT_TYPE_LENGTH_SAMPLES / 2U + DMR_INFO_LENGTH_SAMPLES / 2U - 1U;
          DEBUG3("DMRSlotRX: slot/endPtr", m_slot ? 2U : 1U, m_endPtr);
        }
      }
    }
  }
}

void CDMRSlotRX::samplesToBits(uint16_t start, uint8_t count, uint8_t* buffer, uint16_t offset, q15_t centre, q15_t threshold)
{
  for (uint8_t i = 0U; i < count; i++, start += DMR_RADIO_SYMBOL_LENGTH) {
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
  }
}

void CDMRSlotRX::setColorCode(uint8_t colorCode)
{
  m_colorCode = colorCode;
}

