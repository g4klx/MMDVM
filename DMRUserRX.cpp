/*
 *   Copyright (C) 2009-2017,2020 by Jonathan Naylor G4KLX
 *   Copyright (C) 2025 by Rob Williams M1BGT
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

#if defined(MODE_DMR)

#include "Globals.h"
#include "DMRUserRX.h"
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

const uint8_t cachInterleave[DMR_CACH_LENGTH_BITS] = {
  0, 7, 8, 9, 1, 10, 11, 12, 2, 13, 14,
  15, 3, 16, 4, 17, 18, 19, 5, 20, 21, 22, 6, 23
};

extern bool decodeDMRHamming74(uint8_t *received);

CDMRUserRX::CDMRUserRX() :
m_bitBuffer(),
m_buffer(),
m_bitPtr(0U),
m_dataPtr(0U),
m_endPtr(NOENDPTR),
m_maxCorr(0),
m_centre(0),
m_threshold(0),
m_colorCode(0U),
m_slot(false)
{
}

void CDMRUserRX::reset()
{
  m_dataPtr   = 0U;
  m_bitPtr    = 0U;
  m_maxCorr   = 0;
  m_threshold = 0;
  m_centre    = 0;
  m_slot      = false;
  m_endPtr    = NOENDPTR;
}

void CDMRUserRX::samples(const q15_t* samples, uint8_t length)
{
  for (uint8_t i = 0U; i < length; i++)
    processSample(samples[i]);
}

void CDMRUserRX::processSample(q15_t sample)
{
  m_bitBuffer[m_bitPtr] <<= 1;
  if (sample < 0)
    m_bitBuffer[m_bitPtr] |= 0x01U;

  m_buffer[m_dataPtr] = sample;

  bool bsDataSyncFound = countBits32((m_bitBuffer[m_bitPtr] & DMR_SYNC_SYMBOLS_MASK) ^ DMR_BS_DATA_SYNC_SYMBOLS) <= MAX_SYNC_SYMBOLS_ERRS;
  if (
    bsDataSyncFound
  ) {
    // Sync pattern found

    // | CACH (24 bits/60 samples) | Payload (98 bits) | Slot type (10 bits) | Sync (DMR_SYNC_LENGTH_SAMPLES) | Slot type  Slot type (10 bits) | Payload (98 bits) |

    uint16_t ptr = m_dataPtr + DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES - DMR_SYNC_LENGTH_SAMPLES + DMR_RADIO_SYMBOL_LENGTH;
    if (ptr >= DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES)
      ptr -= DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES;
    // ptr is start of first sync sample

    q31_t corr = 0;
    q15_t max = -16000;
    q15_t min =  16000;

    for (uint8_t i = 0U; i < DMR_SYNC_LENGTH_SYMBOLS; i++) {
      q15_t val = m_buffer[ptr];

      if (val > max)
        max = val;
      if (val < min)
        min = val;

      switch (DMR_BS_DATA_SYNC_SYMBOLS_VALUES[i]) {
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
      if (ptr >= DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES)
        ptr -= DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES;
    }

    if (corr > m_maxCorr) {
      q15_t centre = (max + min) >> 1;

      q31_t v1 = (max - centre) * SCALING_FACTOR;
      q15_t threshold = q15_t(v1 >> 15);

      uint8_t sync[DMR_SYNC_BYTES_LENGTH];

      uint16_t ptr = m_dataPtr + DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES - DMR_SYNC_LENGTH_SAMPLES + DMR_RADIO_SYMBOL_LENGTH;
      if (ptr >= DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES)
        ptr -= DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES;

      samplesToBits(ptr, DMR_SYNC_LENGTH_SYMBOLS, sync, 4U, centre, threshold);

      uint8_t errs = 0U;
      for (uint8_t i = 0U; i < DMR_SYNC_BYTES_LENGTH; i++)
        errs += countBits8((sync[i] & DMR_SYNC_BYTES_MASK[i]) ^  DMR_BS_DATA_SYNC_BYTES[i]);

      if (errs <= MAX_SYNC_BYTES_ERRS) {
        //DEBUG3("DMRUserRX: data sync found centre/threshold", centre, threshold);
        m_maxCorr   = corr;
        m_centre    = centre;
        m_threshold = threshold;

        m_endPtr    = m_dataPtr + DMR_SLOT_TYPE_LENGTH_SAMPLES / 2U + DMR_INFO_LENGTH_SAMPLES / 2U - 1U;
        if (m_endPtr >= DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES)
          m_endPtr -= DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES;
      }
    }
  }

  if (m_dataPtr == m_endPtr) {
    uint16_t ptr = m_endPtr + DMR_RADIO_SYMBOL_LENGTH + 1U + DMR_CACH_LENGTH_SAMPLES;
    if (ptr >= DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES)
	  ptr -= DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES;

    uint16_t cachePtr = m_endPtr + DMR_RADIO_SYMBOL_LENGTH + 1U;
    if (cachePtr >= DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES)
	  cachePtr -= DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES;

    uint8_t frame[DMR_FRAME_LENGTH_BYTES + 1U];
    samplesToBits(ptr, DMR_FRAME_LENGTH_SYMBOLS, frame, 8U, m_centre, m_threshold);

    uint8_t colorCode;
    uint8_t dataType;
    CDMRSlotType slotType;
    slotType.decode(frame + 1U, colorCode, dataType);

    //DEBUG4("At end, cachePtr/ptr/endPtr", cachePtr, ptr, m_endPtr);

    if (colorCode == m_colorCode && dataType != DT_IDLE) {

      // Read the CACH in order to get the time slot
      uint8_t cach[DMR_CACH_LENGTH_BYTES];
      samplesToBits(cachePtr, DMR_CACH_LENGTH_SYMBOLS, cach, 0U, m_centre, m_threshold);
      
      if(cachCheck(cach))
      {
        frame[0U] = CONTROL_DATA | dataType;
        serial.writeDMRData(m_slot, frame, DMR_FRAME_LENGTH_BYTES + 1U);
      } else {
        DEBUG1("CACH check failed");
      }
    }

    m_endPtr  = NOENDPTR;
    m_maxCorr = 0;
  }

  m_dataPtr++;
  if (m_dataPtr >= DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES)
    m_dataPtr = 0U;

  m_bitPtr++;
  if (m_bitPtr >= DMR_RADIO_SYMBOL_LENGTH)
    m_bitPtr = 0U;
}

 #define READ_BIT1(p,i)    ((p[(i)>>3] & BIT_MASK_TABLE[(i)&7]) >> (7 - ((i)&7)))

void CDMRUserRX::extractBits(uint8_t* bytes, uint16_t start, uint8_t count_bits, bool* buffer)
{
    for (uint8_t i = 0U; i < count_bits; i++) {
        buffer[i]  = READ_BIT1(bytes, start) == 0x01;
        start++;
    }
}

bool CDMRUserRX::cachCheck(uint8_t* cach)
{
  uint8_t i;

  bool bits[DMR_CACH_LENGTH_BITS];
  extractBits(cach, 0, DMR_CACH_LENGTH_BITS, bits);

  bool cachdata[DMR_CACH_LENGTH_BITS];
    
  for (i = 0; i < DMR_CACH_LENGTH_BITS; i++)
  {
    cachdata[cachInterleave[i]] = bits[i];
  }

  uint8_t tactBits[7];

  for (i = 0; i < 7; i++)
  {
    tactBits[i] = cachdata[i];
  }

  if(!decodeDMRHamming74(tactBits))
  {
    return false;
  }

  // bool at_continuous = tactBits[0];

  m_slot = tactBits[1]; // TDMA Channel (TC)

  return true;
}

void CDMRUserRX::samplesToBits(uint16_t start, uint8_t count, uint8_t* buffer, uint16_t offset, q15_t centre, q15_t threshold)
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
    if (start >= DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES)
      start -= DMR_FRAME_PLUS_CACH_LENGTH_SAMPLES;
  }
}

void CDMRUserRX::setColorCode(uint8_t colorCode)
{
  m_colorCode = colorCode;
}

#endif

