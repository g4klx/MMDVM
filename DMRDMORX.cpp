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

#define  WANT_DEBUG

#include "Config.h"
#include "Globals.h"
#include "DMRDMORX.h"
#include "DMRSlotType.h"
#include "Utils.h"

const q15_t SCALING_FACTOR = 19505;      // Q15(0.60)

const uint8_t MAX_SYNC_SYMBOLS_ERRS = 2U;
const uint8_t MAX_SYNC_BYTES_ERRS   = 3U;

const uint8_t MAX_SYNC_LOST_FRAMES  = 13U;

const uint8_t BIT_MASK_TABLE[] = {0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U};

#define WRITE_BIT1(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])

const uint16_t NOENDPTR = 9999U;

const uint8_t CONTROL_NONE  = 0x00U;
const uint8_t CONTROL_VOICE = 0x20U;
const uint8_t CONTROL_DATA  = 0x40U;

CDMRDMORX::CDMRDMORX() :
m_bitBuffer(),
m_buffer(),
m_bitPtr(0U),
m_dataPtr(0U),
m_syncPtr(0U),
m_startPtr(0U),
m_endPtr(NOENDPTR),
m_maxCorr(0),
m_centre(),
m_threshold(),
m_averagePtr(0U),
m_control(CONTROL_NONE),
m_syncCount(0U),
m_colorCode(0U),
m_state(DMORXS_NONE),
m_n(0U),
m_type(0U),
m_rssiCount(0U),
m_rssi()
{
}

void CDMRDMORX::reset()
{
  m_syncPtr   = 0U;
  m_maxCorr   = 0;
  m_control   = CONTROL_NONE;
  m_syncCount = 0U;
  m_state     = DMORXS_NONE;
  m_startPtr  = 0U;
  m_endPtr    = NOENDPTR;
  m_rssiCount = 0U;
}

void CDMRDMORX::samples(const q15_t* samples, const uint16_t* rssi, uint8_t length)
{
  bool dcd = false;

  for (uint8_t i = 0U; i < length; i++)
    dcd = processSample(samples[i], rssi[i]);

  io.setDecode(dcd);
}

bool CDMRDMORX::processSample(q15_t sample, uint16_t rssi)
{
  m_buffer[m_dataPtr] = sample;
  m_rssi[m_dataPtr] = rssi;

  m_bitBuffer[m_bitPtr] <<= 1;
  if (sample < 0)
    m_bitBuffer[m_bitPtr] |= 0x01U;

  if (m_state == DMORXS_NONE) {
    correlateSync(true);
  } else {

    uint16_t min  = m_syncPtr + DMO_BUFFER_LENGTH_SAMPLES - 1U;
    uint16_t max  = m_syncPtr + 1U;

    if (min >= DMO_BUFFER_LENGTH_SAMPLES)
      min -= DMO_BUFFER_LENGTH_SAMPLES;
    if (max >= DMO_BUFFER_LENGTH_SAMPLES)
      max -= DMO_BUFFER_LENGTH_SAMPLES;

    if (min < max) {
      if (m_dataPtr >= min && m_dataPtr <= max)
        correlateSync(false);
    } else {
      if (m_dataPtr >= min || m_dataPtr <= max)
        correlateSync(false);
    }
  }

  if (m_dataPtr == m_endPtr) {
    // Find the average centre and threshold values
    q15_t centre    = (m_centre[0U]    + m_centre[1U]    + m_centre[2U]    + m_centre[3U])    >> 2;
    q15_t threshold = (m_threshold[0U] + m_threshold[1U] + m_threshold[2U] + m_threshold[3U]) >> 2;

    uint8_t frame[DMR_FRAME_LENGTH_BYTES + 3U];
    frame[0U] = m_control;

    uint16_t ptr = m_endPtr + DMO_BUFFER_LENGTH_SAMPLES - DMR_FRAME_LENGTH_SAMPLES + DMR_RADIO_SYMBOL_LENGTH + 1U;
    if (ptr >= DMO_BUFFER_LENGTH_SAMPLES)
      ptr -= DMO_BUFFER_LENGTH_SAMPLES;

    samplesToBits(ptr, DMR_FRAME_LENGTH_SYMBOLS, frame, 8U, centre, threshold);
    
    if (m_control == CONTROL_DATA) {
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
            DEBUG4("DMRDMORX: data header found pos/centre/threshold", m_syncPtr, centre, threshold);
            serial.writeDMRData(true, frame, DMR_FRAME_LENGTH_BYTES + 1U);
            m_state = DMORXS_DATA;
            m_type  = 0x00U;
            break;
          case DT_RATE_12_DATA:
          case DT_RATE_34_DATA:
          case DT_RATE_1_DATA:
            if (m_state == DMORXS_DATA) {
              DEBUG4("DMRDMORX: data payload found pos/centre/threshold", m_syncPtr, centre, threshold);
              serial.writeDMRData(true, frame, DMR_FRAME_LENGTH_BYTES + 1U);
              m_type = dataType;
            }
            break;
          case DT_VOICE_LC_HEADER:
            DEBUG4("DMRDMORX: voice header found pos/centre/threshold", m_syncPtr, centre, threshold);
            serial.writeDMRData(true, frame, DMR_FRAME_LENGTH_BYTES + 1U);
            m_state = DMORXS_VOICE;
            break;
          case DT_VOICE_PI_HEADER:
            if (m_state == DMORXS_VOICE) {
              DEBUG4("DMRDMORX: voice pi header found pos/centre/threshold", m_syncPtr, centre, threshold);
              serial.writeDMRData(true, frame, DMR_FRAME_LENGTH_BYTES + 1U);
            }
            m_state = DMORXS_VOICE;
            break;
          case DT_TERMINATOR_WITH_LC:
            if (m_state == DMORXS_VOICE) {
              DEBUG4("DMRDMORX: voice terminator found pos/centre/threshold", m_syncPtr, centre, threshold);
              serial.writeDMRData(true, frame, DMR_FRAME_LENGTH_BYTES + 1U);
              reset();
            }
            break;
          default:    // DT_CSBK
            DEBUG4("DMRDMORX: csbk found pos/centre/threshold", m_syncPtr, centre, threshold);
            serial.writeDMRData(true, frame, DMR_FRAME_LENGTH_BYTES + 1U);
            reset();
            break;
        }
      }
    } else if (m_control == CONTROL_VOICE) {
      // Voice sync
      DEBUG4("DMRDMORX: voice sync found pos/centre/threshold", m_syncPtr, centre, threshold);
#if defined(SEND_RSSI_DATA)
      // Send RSSI data approximately every second
      if (m_rssiCount == 2U) {
        // Calculate RSSI average over a burst period. We don't take into account 2.5 ms at the beginning and 2.5 ms at the end
        uint16_t rssi_avg = avgRSSI(m_startPtr + DMR_SYNC_LENGTH_SAMPLES / 2U, DMR_FRAME_LENGTH_SAMPLES - DMR_SYNC_LENGTH_SAMPLES);
        frame[34U] = (rssi_avg >> 8) & 0xFFU;
        frame[35U] = (rssi_avg >> 0) & 0xFFU;
        serial.writeDMRData(true, frame, DMR_FRAME_LENGTH_BYTES + 3U);
      } else {
        serial.writeDMRData(true, frame, DMR_FRAME_LENGTH_BYTES + 1U);
      }
      
      m_rssiCount++;
      if (m_rssiCount >= 16U)
        m_rssiCount = 0U;
#else
      serial.writeDMRData(true, frame, DMR_FRAME_LENGTH_BYTES + 1U);
#endif
      m_state     = DMORXS_VOICE;
      m_syncCount = 0U;
      m_n         = 0U;
    } else {
      if (m_state != DMORXS_NONE) {
        m_syncCount++;
        if (m_syncCount >= MAX_SYNC_LOST_FRAMES) {
          serial.writeDMRLost(true);
          reset();
        }
      }

      if (m_state == DMORXS_VOICE) {
        if (m_n >= 5U) {
          frame[0U] = CONTROL_VOICE;
          m_n = 0U;
        } else {
          frame[0U] = ++m_n;
        }
#if defined(SEND_RSSI_DATA)
        // Send RSSI data approximately every second
        if (m_rssiCount == 2U) {
          // Calculate RSSI average over a burst period. We don't take into account 2.5 ms at the beginning and 2.5 ms at the end
          uint16_t rssi_avg = avgRSSI(m_startPtr + DMR_SYNC_LENGTH_SAMPLES / 2U, DMR_FRAME_LENGTH_SAMPLES - DMR_SYNC_LENGTH_SAMPLES);
          frame[34U] = (rssi_avg >> 8) & 0xFFU;
          frame[35U] = (rssi_avg >> 0) & 0xFFU;
          serial.writeDMRData(true, frame, DMR_FRAME_LENGTH_BYTES + 3U);
        } else {
          serial.writeDMRData(true, frame, DMR_FRAME_LENGTH_BYTES + 1U);
        }
        
        m_rssiCount++;
        if (m_rssiCount >= 16U)
          m_rssiCount = 0U;
#else
        serial.writeDMRData(true, frame, DMR_FRAME_LENGTH_BYTES + 1U);
#endif
      } else if (m_state == DMORXS_DATA) {
        if (m_type != 0x00U) {
          frame[0U] = CONTROL_DATA | m_type;
          serial.writeDMRData(true, frame, DMR_FRAME_LENGTH_BYTES + 1U);
        }
      }
    }

    // End of this slot, reset some items for the next slot.
    m_maxCorr = 0;
    m_control = CONTROL_NONE;
  }

  m_dataPtr++;
  if (m_dataPtr >= DMO_BUFFER_LENGTH_SAMPLES)
    m_dataPtr = 0U;

  m_bitPtr++;
  if (m_bitPtr >= DMR_RADIO_SYMBOL_LENGTH)
    m_bitPtr = 0U;

  return m_state != DMORXS_NONE;
}

void CDMRDMORX::correlateSync(bool first)
{
  uint8_t errs1 = countBits32((m_bitBuffer[m_bitPtr] & DMR_SYNC_SYMBOLS_MASK) ^ DMR_S2_DATA_SYNC_SYMBOLS);
  uint8_t errs2 = countBits32((m_bitBuffer[m_bitPtr] & DMR_SYNC_SYMBOLS_MASK) ^ DMR_MS_DATA_SYNC_SYMBOLS);

  // The voice sync is the complement of the data sync
  bool data1  = (errs1 <= MAX_SYNC_SYMBOLS_ERRS);
  bool data2  = (errs2 <= MAX_SYNC_SYMBOLS_ERRS);
  bool voice1 = (errs1 >= (DMR_SYNC_LENGTH_SYMBOLS - MAX_SYNC_SYMBOLS_ERRS));
  bool voice2 = (errs2 >= (DMR_SYNC_LENGTH_SYMBOLS - MAX_SYNC_SYMBOLS_ERRS));

  if (data1 || data2 || voice1 || voice2) {
    uint16_t ptr = m_dataPtr + DMO_BUFFER_LENGTH_SAMPLES - DMR_SYNC_LENGTH_SAMPLES + DMR_RADIO_SYMBOL_LENGTH;
    if (ptr >= DMO_BUFFER_LENGTH_SAMPLES)
      ptr -= DMO_BUFFER_LENGTH_SAMPLES;

    q31_t corr = 0;
    q15_t min =  16000;
    q15_t max = -16000;

    uint32_t mask = 0x00800000U;
    for (uint8_t i = 0U; i < DMR_SYNC_LENGTH_SYMBOLS; i++, mask >>= 1) {
      bool b;
      if (data1 || voice1)
        b = (DMR_S2_DATA_SYNC_SYMBOLS & mask) == mask;
      else
        b = (DMR_MS_DATA_SYNC_SYMBOLS & mask) == mask;

      if (m_buffer[ptr] > max)
        max = m_buffer[ptr];
      if (m_buffer[ptr] < min)
        min = m_buffer[ptr];

      if (data1 || data2)
        corr += b ? -m_buffer[ptr] : m_buffer[ptr];
      else  // if (voice)
        corr += b ? m_buffer[ptr] : -m_buffer[ptr];

      ptr += DMR_RADIO_SYMBOL_LENGTH;
      if (ptr >= DMO_BUFFER_LENGTH_SAMPLES)
        ptr -= DMO_BUFFER_LENGTH_SAMPLES;
    }

    if (corr > m_maxCorr) {
      q15_t centre = (max + min) >> 1;

      q31_t v1 = (max - centre) * SCALING_FACTOR;
      q15_t threshold = q15_t(v1 >> 15);

      uint8_t sync[DMR_SYNC_BYTES_LENGTH];
      uint16_t ptr = m_dataPtr + DMO_BUFFER_LENGTH_SAMPLES - DMR_SYNC_LENGTH_SAMPLES + DMR_RADIO_SYMBOL_LENGTH;
      if (ptr >= DMO_BUFFER_LENGTH_SAMPLES)
        ptr -= DMO_BUFFER_LENGTH_SAMPLES;

      samplesToBits(ptr, DMR_SYNC_LENGTH_SYMBOLS, sync, 4U, centre, threshold);

      if (data1 || data2) {
        uint8_t errs = 0U;
        for (uint8_t i = 0U; i < DMR_SYNC_BYTES_LENGTH; i++) {
          if (data1)
            errs += countBits8((sync[i] & DMR_SYNC_BYTES_MASK[i]) ^ DMR_S2_DATA_SYNC_BYTES[i]);
          else
            errs += countBits8((sync[i] & DMR_SYNC_BYTES_MASK[i]) ^ DMR_MS_DATA_SYNC_BYTES[i]);
        }

        if (errs <= MAX_SYNC_BYTES_ERRS) {
          if (first) {
            m_threshold[0U] = m_threshold[1U] = m_threshold[2U] = m_threshold[3U] = threshold;
            m_centre[0U]    = m_centre[1U]    = m_centre[2U]    = m_centre[3U]    = centre;
            m_averagePtr    = 0U;
            m_rssiCount     = 0U;
          } else {
            m_threshold[m_averagePtr] = threshold;
            m_centre[m_averagePtr]    = centre;

            m_averagePtr++;
            if (m_averagePtr >= 4U)
              m_averagePtr = 0U;
          }

          m_maxCorr  = corr;
          m_control  = CONTROL_DATA;
          m_syncPtr  = m_dataPtr;

          m_startPtr = m_dataPtr + DMO_BUFFER_LENGTH_SAMPLES - DMR_SLOT_TYPE_LENGTH_SAMPLES / 2U - DMR_INFO_LENGTH_SAMPLES / 2U - DMR_SYNC_LENGTH_SAMPLES;
          if (m_startPtr >= DMO_BUFFER_LENGTH_SAMPLES)
            m_startPtr -= DMO_BUFFER_LENGTH_SAMPLES;

          m_endPtr = m_dataPtr + DMR_SLOT_TYPE_LENGTH_SAMPLES / 2U + DMR_INFO_LENGTH_SAMPLES / 2U - 1U;
          if (m_endPtr >= DMO_BUFFER_LENGTH_SAMPLES)
            m_endPtr -= DMO_BUFFER_LENGTH_SAMPLES;
        }
      } else {  // if (voice1 || voice2)
        uint8_t errs = 0U;
        for (uint8_t i = 0U; i < DMR_SYNC_BYTES_LENGTH; i++) {
          if (voice1)
            errs += countBits8((sync[i] & DMR_SYNC_BYTES_MASK[i]) ^ DMR_S2_VOICE_SYNC_BYTES[i]);
          else
            errs += countBits8((sync[i] & DMR_SYNC_BYTES_MASK[i]) ^ DMR_MS_VOICE_SYNC_BYTES[i]);
        }

        if (errs <= MAX_SYNC_BYTES_ERRS) {
          if (first) {
            m_threshold[0U] = m_threshold[1U] = m_threshold[2U] = m_threshold[3U] = threshold;
            m_centre[0U]    = m_centre[1U]    = m_centre[2U]    = m_centre[3U]    = centre;
            m_averagePtr    = 0U;
            m_rssiCount     = 0U;
          } else {
            m_threshold[m_averagePtr] = threshold;
            m_centre[m_averagePtr]    = centre;

            m_averagePtr++;
            if (m_averagePtr >= 4U)
              m_averagePtr = 0U;
          }

          m_maxCorr  = corr;
          m_control  = CONTROL_VOICE;
          m_syncPtr  = m_dataPtr;

          m_startPtr = m_dataPtr + DMO_BUFFER_LENGTH_SAMPLES - DMR_SLOT_TYPE_LENGTH_SAMPLES / 2U - DMR_INFO_LENGTH_SAMPLES / 2U - DMR_SYNC_LENGTH_SAMPLES;
          if (m_startPtr >= DMO_BUFFER_LENGTH_SAMPLES)
            m_startPtr -= DMO_BUFFER_LENGTH_SAMPLES;

          m_endPtr   = m_dataPtr + DMR_SLOT_TYPE_LENGTH_SAMPLES / 2U + DMR_INFO_LENGTH_SAMPLES / 2U - 1U;
          if (m_endPtr >= DMO_BUFFER_LENGTH_SAMPLES)
            m_endPtr -= DMO_BUFFER_LENGTH_SAMPLES;
        }
      }
    }
  }
}

uint16_t CDMRDMORX::avgRSSI(uint16_t start, uint16_t count)
{
  float rssi_tmp = 0.0F;

  for (uint16_t i = 0U; i < count; i++) {
    rssi_tmp += float(m_rssi[start]);

    start++;
    if (start >= DMO_BUFFER_LENGTH_SAMPLES)
      start -= DMO_BUFFER_LENGTH_SAMPLES;
  }

  return uint16_t(rssi_tmp / count);
}

void CDMRDMORX::samplesToBits(uint16_t start, uint8_t count, uint8_t* buffer, uint16_t offset, q15_t centre, q15_t threshold)
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
    if (start >= DMO_BUFFER_LENGTH_SAMPLES)
      start -= DMO_BUFFER_LENGTH_SAMPLES;
  }
}

void CDMRDMORX::setColorCode(uint8_t colorCode)
{
  m_colorCode = colorCode;
}

